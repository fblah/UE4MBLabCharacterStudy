import cv2
import numpy as np

import socket
import os
import sys
import errno
from imutils import face_utils
import dlib
import time
from pydispatch import dispatcher
import threading
from lshash import lshash 



UDPSERVER_SIGNAL='udpserver_signal'
UDPSERVER_SENDER='udpserver_sender'
CV_SIGNAL='cv_signal'
CV_SENDER='cv_sender'


class StoppableThread(threading.Thread):
    """Thread class with a stop() method. The thread itself has to check
    regularly for the stopped() condition."""

    def __init__(self, *args, **kwargs):
        super(StoppableThread, self).__init__(*args, **kwargs)
        self._stop = threading.Event()

    def stop(self):
        self._stop.set()

    def stopped(self):
        return self._stop.isSet()

class UDPServer(StoppableThread):    
    def __init__(self, *args, **kwargs):                
        super(UDPServer, self).__init__(*args, **kwargs)
        self._stop_event = threading.Event()
        dispatcher.connect(self.UDPServer_dispatcher_receive, signal=CV_SIGNAL, sender=CV_SENDER)        
        self.host = ''
        self.port = 8600
        self.addr = 0
        self.messages = []
        self.create_socket()
       
    def UDPServer_dispatcher_receive(self, message):
        ''' handle dispatcher'''
        if(message == "reset"):
            self.addr = 0
            self.create_socket()
        
        self.messages.append(message)
        dispatcher.send(message='message received', signal=UDPSERVER_SIGNAL, sender=UDPSERVER_SENDER)
    
    def create_socket(self):
        # Create the UDP Socket
        try:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)    
            #s.setblocking(false)
            self.s.settimeout(.02)
            print("Socket Created")
        except:
            print("Failed to create socket.")
            self.stop()
        # Bind socket to local host and port
        try:
            self.s.bind((self.host, self.port))
        except:
            print("Bind failed.")
            self.stop()
            return
        print("Socket bind complete.")
    
    def update_socket(self):        
               
        try:
            # Send Message
            if(len(self.messages)):            
                try:
                    msg = self.messages.pop(0)
                    self.s.sendto(bytes(msg, 'utf-8'), self.addr)
                    #print(msg)
                except TypeError as e:            
                    self.addr = 0 
            d, a = self.s.recvfrom(1024)            
        except socket.error as e:
            err = e.args[0]
            if err == errno.EAGAIN or err == errno.EWOULDBLOCK or err == 'timed out':                
                #print(err)
                return
            else:
                # a "real" error occurred
                print( e)
                self.stop()
                return         
        else:            
            # Print incoming message
            self.addr = a
            d = d.decode('UTF-8')
            if(d != 'Hi'):
                print(d)
            if("set_morph_name" in d or "save_face" in d or "save_data" in d):
                dispatcher.send(message=d, signal=UDPSERVER_SIGNAL, sender=UDPSERVER_SENDER)        
        
                
    def run(self):
        ''' loop and wait '''        
        while(1):
            self.update_socket()
            if(self.stopped()):
                print("UDP server closed")
                return

#Computer Vision
class CV_FaceDetection(threading.Thread):
    
    
    def __init__(self, udp_thread):                                
        dispatcher.connect(self.dispatcher_receive, signal=UDPSERVER_SIGNAL, sender=UDPSERVER_SENDER)
        # initialize dlib's face detector (HOG-based) and then create
        # the facial landmark predictor
        self.detector = dlib.get_frontal_face_detector()
        self.predictor = dlib.shape_predictor("shape_predictor_68_face_landmarks.dat")
        self.camera_index = 0
        self.cap = cv2.VideoCapture(self.camera_index)
        self.count = 0
        self.old_time = 0
        self.dimensions = 26
        self.reference_scale_multiplier = 5
        self.lsh = lshash.LSHash(256, self.dimensions)
        self.extra_data = ""
        self.save_face = False
        self.save_file_name = "face_data3.json"
        self.imageScale = 0.25
        
        self.udp_thread = udp_thread
        self.faceDetection()        
        
        
    def dispatcher_receive(self, message):
        ''' handle dispatcher'''
        if("set_morph_name" in message):
            self.extra_data = message.split(":")[1]
        elif("save_face" in message):
            self.save_face = True
        elif("save_data" in message):
            self.lsh.save_data("data.json")
        #print("OK")        
            
    def __del__(self):        
        self.cap.release()
        cv2.destroyAllWindows()
        time.sleep(2)
    
    def faceDetection(self):        
        while True:
            #print(time.time() - self.old_time)
            #self.old_time = time.time()
            _, image = self.cap.read()    
            gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            
            small = cv2.resize(gray, (0,0), fx=self.imageScale, fy=self.imageScale)            
            if(self.count % 4 == 0):
                rects = self.detector(small, 1)           
            self.count += 1
            messages = ""
            data = []
            # loop over the face detections
            if(len(rects)):
                rect = rects[0]
                #get points from the predictor
# this part is important
                faceRectangle = dlib.rectangle(int(rect.left() / self.imageScale), int(rect.top() / self.imageScale), int(rect.right() / self.imageScale), int(rect.bottom() / self.imageScale))
                shape = self.predictor(gray, faceRectangle)                
                shape = face_utils.shape_to_np(shape)
## till here
                data = []                
                x = np.array(shape[27] - shape[30])                
                reference_scale_i = 1 / (np.sqrt(x.dot(x)) * self.reference_scale_multiplier)                                
                #side of lips               

                for i in [5,9,11,48,50,52,54,56,58,61,63,65,67]:#range(50,67):
                    x = np.array(shape[27] - shape[i])
                    y = list(map(lambda z: ((z) * reference_scale_i), x))
                    data += y
                
                #print(len(data), data)                            
                messages="shapes|"
                # loop over the (x, y)-coordinates for the facial landmarks
                # and draw them on the image
                
                for (x, y) in shape:
                    #messages +=(str(x)+","+str(y)+";")
                    cv2.circle(image, (x, y), 3, (0, 0, 255), -1)            
                
            #check if 5 delta data is available and store            
            temp = []
            if(len(data)):
                temp = self.lsh.query(data,num_results= 3 )
                
            if(len(data)== self.dimensions):            
                messages="mouth|"                
                for i in temp:
                    if(len(i)==2):                        
                        c = i[0][1].split("|")
                        #print(c,i[1]*1000)
                        if(len(c)==2):
                            messages += str(c[0]) + "|" + str(float(c[1])*(1- min(1.0, i[1])*.1)) + "+" + str(i[1]) + ";"
                        else:
                            messages += str(i[0][1])+ "+" + str(i[1]) + ";"
                messages = messages[0:len(messages)-1]
                
                if(len(temp)):
                    print(messages)
                    dispatcher.send(message=messages, signal=CV_SIGNAL, sender=CV_SENDER)
                
                if(self.save_face):
                    self.lsh.index(data,extra_data=self.extra_data)
                    self.save_face = False
                    dispatcher.send(message="saved_face", signal=CV_SIGNAL, sender=CV_SENDER)
            
            # show the output image with the face detections + facial landmarks
            cv2.imshow("Output", image)           
            
            key = cv2.waitKey(1)
            if key == 27:                
                self.udp_thread.stop()                
                break
            if key == 32:
                key = cv2.waitKey(0)
                dispatcher.send(message="reset", signal=CV_SIGNAL, sender=CV_SENDER)
                if key == 83 or key == 115: #s
                    if(len(data)== self.dimensions):
                        self.lsh.index(data,extra_data="Default")
                        print("added default face")
                if key == 65 or key == 97: #a save face
                    if(len(data)== self.dimensions):
                        self.lsh.index(data,extra_data=self.extra_data)
                        print("add face")    
                if key == 78 or key == 110: #n next face in ue
                    dispatcher.send(message="next_face|pls", signal=CV_SIGNAL, sender=CV_SENDER)
                    print("next face")
            if key == 83 or key == 115: #s
                if(len(data)== self.dimensions):
                    self.lsh.index(data,extra_data="Default")
                    print("Saved")
            elif key == 82 or key == 114: #r print values
                for i in temp:
                    print(i[0][1],i[1])
            elif key == 84 or key == 116: #t save                   
                self.lsh.save_data(self.save_file_name)
                print("saved")
            elif key == 85 or key == 117: #u load
                self.lsh.load_data(self.save_file_name)
                print("loaded")
            elif key == 78 or key == 110: #n next face in ue
                dispatcher.send(message="next_face|pls", signal=CV_SIGNAL, sender=CV_SENDER)
                print("next face")
            elif key == 65 or key == 97: #a save face
                if(len(data)== self.dimensions):
                    self.lsh.index(data,extra_data=self.extra_data)
                    print("add face")
            elif key == 77 or key == 109: #m reset
                dispatcher.send(message="reset", signal=CV_SIGNAL, sender=CV_SENDER)
                print("reset")                
                
udp_thread = UDPServer()
udp_thread.start()
CV_FaceDetection(udp_thread)
