#pragma once

#include "Object.h"
#include "Networking.h"
#include "Json.h"
#include "UDPReceiveWorker.h"
#include "UDPNetworkingWrapper.generated.h"


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MBLAB_FBLAH_STUDY_API UUDPNetworkingWrapper : public UObject
{
	GENERATED_BODY()

public:
	//////////////////////////////////////////////////////////////////////////
	// Construction

	/**
	* Creates and initializes a new UDPNetworking object.
	*
	* @param Description - The description of the socket for debug purposes.
	* @param SenderSocketName - Name of the sender socket for debug purposes.
	* @param TheIP - IP of the the machine you want to send a message too.
	* @param ThePort - The port of the machine you are trying to talk to.
	* @param BufferSize - The size of the buffer for the socket
	* @param AllowBroadCast - Allow broadcasting on this socket?
	* @param Bound - Bind socket to the ip and port?
	* @param Reusable - Is this socket reusable?
	* @param Blocking - Is this socket blocking other data?
	*/
	UFUNCTION(BlueprintPure, Category = "UDPNetworking")
		static UUDPNetworkingWrapper* ConstructUDPWrapper(const FString& Description, const FString& SenderSocketName, const FString& TheIP, const int32 ThePort, const int32 BufferSize,
			const bool AllowBroadcast, const bool Bound, const bool Reusable, const bool Blocking);

	static UUDPNetworkingWrapper* Constructor();

	/**
	* Sends the supplied message
	* @param Message The message to be sent.
	*/
	UFUNCTION(BlueprintCallable, Category = "UDPNetworking")
		bool sendMessage(FString Message);

	//////////////////////////////////////////////////////////////////////////
	// Destruction and reset

	/** Destroys all data */
	UFUNCTION(BlueprintCallable, Category = "UDPNetworking")
		void UDPDestructor();

	//// Grab Data
	UFUNCTION(BlueprintCallable, Category = "UDPNetworking")
		bool anyMessages();


	/** Test Look for message */
	UFUNCTION(BlueprintCallable, Category = "UDPNetworking")
		FString GrabWaitingMessage();

	static FString StringFromBinaryArray(const TArray<uint8>&  BinaryArray);

private:

	// Holds the socket we are sending on
	FSocket * SenderSocket;

	// Description for debugging
	FString SocketDescription;

	// Remote Address
	FIPv4Endpoint RemoteEndPoint;
	FIPv4Address RemoteAdress;

	// Socket Subsystem
	ISocketSubsystem* SocketSubsystem;

	//UDPReceiveWorker* myRecieverWorker;
	// The data
	TArray<uint8> ReceivedData;	
};
