
#include "BackendInterface.h"
#include "SefaPool/Widgets/LoadingWidget.h"

void UBackendInterface::Init(USefaPoolGameInstance* gi_)
{
    gi = gi_;
    lobby = NewObject<UBackendLobbyInterface> (gi_);
    lobby->Init(gi_);
}
