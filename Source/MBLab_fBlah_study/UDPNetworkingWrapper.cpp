#include "UDPNetworkingWrapper.h"
#include "MBLab_fBlah_study.h"


UUDPNetworkingWrapper* UUDPNetworkingWrapper::Constructor()
{
	return (UUDPNetworkingWrapper*)StaticConstructObject_Internal(UUDPNetworkingWrapper::StaticClass());
}
UUDPNetworkingWrapper* UUDPNetworkingWrapper::ConstructUDPWrapper(const FString& Description, const FString& SenderSocketName, const FString& TheIP, const int32 ThePort, const int32 BufferSize,
	const bool AllowBroadcast, const bool Bound, const bool Reusable, const bool Blocking)
{

	UUDPNetworkingWrapper* wrapper = Constructor();

	wrapper->SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	FIPv4Address::Parse(TheIP, wrapper->RemoteAdress);

	wrapper->RemoteEndPoint = FIPv4Endpoint(wrapper->RemoteAdress, ThePort);

	// First set our socket null
	wrapper->SenderSocket = nullptr;

	if (wrapper->SocketSubsystem != nullptr) // If socket subsytem is good
	{
		wrapper->SenderSocket = wrapper->SocketSubsystem->CreateSocket(NAME_DGram, *Description, true);

		if (wrapper->SenderSocket != nullptr) // Is our socket created
		{
			// Setup the socket 
			bool Error = !wrapper->SenderSocket->SetNonBlocking(!Blocking) ||
				!wrapper->SenderSocket->SetReuseAddr(Reusable) ||
				!wrapper->SenderSocket->SetBroadcast(AllowBroadcast) ||
				!wrapper->SenderSocket->SetRecvErr();

			if (!Error)
			{
				if (Bound)
				{
					Error = !wrapper->SenderSocket->Bind(*wrapper->RemoteEndPoint.ToInternetAddr());
				}
			}

			if (!Error)
			{
				int32 OutNewSize;

				wrapper->SenderSocket->SetReceiveBufferSize(BufferSize, OutNewSize);
				wrapper->SenderSocket->SetSendBufferSize(BufferSize, OutNewSize);
			}

			if (Error)
			{
				GLog->Logf(TEXT("FUdpSocketBuilder: Failed to create the socket %s as configured"), *Description);

				wrapper->SocketSubsystem->DestroySocket(wrapper->SenderSocket);
				wrapper->SenderSocket = nullptr;
			}
		}

	}


	return wrapper;
}

bool UUDPNetworkingWrapper::sendMessage(FString Message)
{
	if (!SenderSocket) return false;

	int32 BytesSent;
	FTimespan waitTime = FTimespan(10);
	TCHAR *serializedChar = Message.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);
	int32 sent = 0;

	// Send to
	//myRecieverWorker = UDPReceiveWorker::JoyInit(SenderSocket, waitTime);
	bool success = SenderSocket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteEndPoint.ToInternetAddr());
	if (success && BytesSent > 0) // Success
	{
		return true;
	}
	else
	{
		return false;
	}
}

FString UUDPNetworkingWrapper::GrabWaitingMessage()
{
	uint32 Size;

	TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();

	while (SenderSocket->HasPendingData(Size))
	{
		int32 Read = 0;
		ReceivedData.Init(0, FMath::Min(Size, 65507u));
		SenderSocket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), Read, *Sender);
	}


	return StringFromBinaryArray(ReceivedData);

}

bool UUDPNetworkingWrapper::anyMessages()
{

	uint32 Size;

	if (SenderSocket->HasPendingData(Size))
	{
		return true;
	}

	return false;
}

FString UUDPNetworkingWrapper::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	const std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());

	//FString can take in the c_str() of a std::string
	return FString(cstr.c_str());

}

void UUDPNetworkingWrapper::UDPDestructor()
{
	SocketSubsystem->DestroySocket(SenderSocket);
	SenderSocket = nullptr;
	SocketSubsystem = nullptr;
	//myRecieverWorker = nullptr;
}