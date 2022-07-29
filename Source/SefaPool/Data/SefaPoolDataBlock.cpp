
#include "SefaPoolDataBlock.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

void USefaPoolDataBlock::Init(USefaPoolGameInstance* gi_)
{
    gi = gi_;
}
    
void USefaPoolDataBlock::Save()
{
    
}

void USefaPoolDataBlock::Load()
{
    
}

void USefaPoolDataBlock::Pull()
{
    PullWithCallbacks(nullptr, nullptr);
}

void USefaPoolDataBlock::PullWithDelegates(const FResponseDelegat& successful,
                                           const FResponseDelegat& unsuccessful)
{
    PullWithCallbacks([successful, unsuccessful](USefaPoolDataBlock*){successful.Execute();},
                      [successful, unsuccessful](USefaPoolDataBlock*){unsuccessful.Execute();});
}

void USefaPoolDataBlock::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& succesful,
                                           const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    
}
