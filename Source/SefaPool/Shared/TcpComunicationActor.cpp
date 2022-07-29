// Fill out your copyright notice in the Description page of Project Settings.


#include "TcpComunicationActor.h"

static int count_listener = 0;
static int count_getter = 0;

// Called when the game starts or when spawned
void ATcpComunicationActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATcpComunicationActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATcpComunicationActor::set_handle_query(std::function<void(uint8* ReceivedData, uint32 Size)> handle_query)
{
	handle_query_ = handle_query;
}

bool ATcpComunicationActor::StartTCPReceiver(
	const FString& SocketName,
	const FString& TheIP,
	const int32 ThePort
) {
	//Rama's CreateTCPConnectionListener
	CreateTCPConnectionListener(SocketName, TheIP, ThePort);

	//Not created?
	if (!ListenerSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartTCPReceiver>> Listen socket could not be created! ~> %s %d"), *TheIP, ThePort);
		return false;
	}

	//Start the Listener! //thread this eventually
	GetWorldTimerManager().SetTimer(ConnectionTimer, this,
		&ATcpComunicationActor::TCPConnectionListener, 0.01, true);

	return true;
}

//Rama's Create TCP Connection Listener
bool ATcpComunicationActor::CreateTCPConnectionListener(const FString& SocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize)
{
	FIPv4Address IpAddress;
	FIPv4Address::Parse(TheIP, IpAddress);
	FIPv4Endpoint Endpoint(IpAddress, StaticCast<uint32>(ThePort));
	Endpoint.Initialize();
	ListenerSocket = FTcpSocketBuilder(*SocketName)
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenerSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);

	//Done!
	return !(!ListenerSocket);
}

void ATcpComunicationActor::TCPConnectionListener()
{
	//~~~~~~~~~~~~~
	if (!ListenerSocket) return;
	//~~~~~~~~~~~~~

	//Remote address
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Pending;

	// handle incoming connections
	if (ListenerSocket->HasPendingConnection(Pending) && Pending)
	{
		UE_LOG(LogTemp, Warning, TEXT("Get connection."));

		if (ConnectionSocket)
		{
			ConnectionSocket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		}

		ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("TCP Received Socket Connection"));

		if (ConnectionSocket != NULL)
		{
			RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);
			UE_LOG(LogTemp, Warning, TEXT("Start listening."));
			//can thread this too
			GetWorldTimerManager().SetTimer(ListenerTimer, this,
				&ATcpComunicationActor::TCPSocketListener, 0.01, true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Something wrong with recieved socket."));
		}
	}
}

//Rama's TCP Socket Listener
void ATcpComunicationActor::TCPSocketListener()
{

	if (!ConnectionSocket)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Something wrong with recieved socket while listening."));
		return;
	}
	uint32 Size = 0;
	int32 Read = 0;
	uint8* ReceivedData = nullptr;
	if (ConnectionSocket->HasPendingData(Size))
	{
		ReceivedData = new uint8[Size];
		if (!ConnectionSocket->Recv(ReceivedData, FMath::Min(Size, 65507u), Read))
		{
			UE_LOG(LogTemp, Warning, TEXT("Error when recieving data."));
		}
	}

	if (Read <= 0)
	{	
		return;
	}
	handle_query_(ReceivedData, Size);
	delete[] ReceivedData;
}

FString ATcpComunicationActor::StringFromBinaryArray(uint8 *BinaryArray)
{
	const std::string cstr(reinterpret_cast<const char*>(BinaryArray));
	return FString(cstr.c_str());
}

bool SendMessege(uint8* bsring) {
	return false;
}
