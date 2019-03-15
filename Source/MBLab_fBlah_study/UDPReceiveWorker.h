#pragma once
#include "Networking.h"

#include <string>

/**
 * 
 */
class MBLAB_FBLAH_STUDY_API UDPReceiveWorker : public FRunnable
{
	// Singleton instance, can access the thread at anytime.
	static UDPReceiveWorker* Runnable;

	// The data
	TArray<uint8>* ReceivedDataRef;

	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;




public:
	UDPReceiveWorker(TArray<uint8>& ReceivedData, FSocket* InSocket, FTimespan& InWaitTime);
	virtual ~UDPReceiveWorker();



	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface

	void EnsureCompletion();

	static UDPReceiveWorker* JoyInit(TArray<uint8>& ReceivedData, FSocket* InSocket, FTimespan& InWaitTime);

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();

private:

	// Holds the network socket.
	FSocket * Socket;

	// Holds a pointer to the socket sub-system.
	ISocketSubsystem* SocketSubsystem;

	// Holds a flag indicating that the thread is stopping.
	bool Stopping;

	// Holds the thread object.
	FRunnableThread* Thread;

	// Holds the amount of time to wait for inbound packets.
	FTimespan WaitTime;

};
