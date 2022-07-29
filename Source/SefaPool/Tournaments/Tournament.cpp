// Fill out your copyright notice in the Description page of Project Settings.


#include "Tournament.h"
#include "BaseTournamentGameMode.h"
#include "TournamentGameState.h"

void UTournamentMatch::Init(UTournament* tournament, int match_id, int stage)
{
    //participating_user_id.Reserve(2);
    //UE_LOG(LogTemp, Warning, TEXT("in match with id: %d participating_user_id count is %d"), match_id_, participating_user_id.Num());
    //participating_user_id.Reset();
    UE_LOG(LogTemp, Warning, TEXT("in match with id: %d participating_user_id count is %d"), match_id_, PlayersInfo.Num());
    PlayersInfo.Reset();
    tournament_ = tournament;
    match_id_ = match_id;
    stage_ = stage;
};

bool UTournamentMatch::AddPlayer(FString user_id) {
    UE_LOG(LogTemp, Display, TEXT("Adding user with id: %s to match with id: %d"), *user_id, match_id_);
    //UE_LOG(LogTemp, Warning, TEXT("in match with id: %d participating_user_id count is %d"), match_id_, participating_user_id.Num());
    UE_LOG(LogTemp, Warning, TEXT("in match with id: %d participating_user_id count is %d"), match_id_, PlayersInfo.Num());

    if (!IsValid(this)) {
        TRACE("Current match is not valid can't add player");
        return false;
    }

    //if (participating_user_id.Num() == 2) {
    if (PlayersInfo.Num() == 2) {
        //UE_LOG(LogTemp, Warning, TEXT("Match with id: %d is already constructed with user ids: %s, %s. Can't add user_id %s"), match_id_, *participating_user_id[0], *participating_user_id[1], *user_id);
        UE_LOG(LogTemp, Warning, TEXT("Match with id: %d is already constructed with user ids: %s, %s. Can't add user_id %s"), match_id_, *PlayersInfo[0].participating_user_id, *PlayersInfo[1].participating_user_id, *user_id);
        return false;
    }

    /*
    if (participating_user_id.Contains(user_id))  {
        UE_LOG(LogTemp, Warning, TEXT("Match with id: %d is already add user with id: %s. Can't add user_id %s"), match_id_, *participating_user_id[0], *user_id);
        return false;
    }
    */

    if ((PlayersInfo.Num() == 1) && PlayersInfo[0].participating_user_id == user_id) {
        UE_LOG(LogTemp, Warning, TEXT("Match with id: %d is already add user with id: %s. Can't add user_id %s"), match_id_, *PlayersInfo[0].participating_user_id, *user_id);
        return false;
    }

    ATournamentGameState* gs = GetWorld()->GetGameState<ATournamentGameState>();
    if (!gs) {
        UE_LOG(LogTemp, Warning, TEXT("in match with id: %d is not correct TournamentGameState"), match_id_);
        return false;
    }

    gs->tournament_info[match_id_].AddPlayer(user_id, tournament_->userid_to_username);

    tournament_->userid_to_match.Add(user_id, this);

    FPlayerInfo_TournamentMatch AddedPlayer;
    AddedPlayer.participating_user_id = user_id;
    AddedPlayer.accepted_user_id = false;
    AddedPlayer.ready_user_id = true;

    PlayersInfo.Add(AddedPlayer);
    //participating_user_id.Add(user_id);
    UE_LOG(LogTemp, Display, TEXT("Participating user_id: "));
    
    //for (const auto& id : participating_user_id) {
    //    UE_LOG(LogTemp, Display, TEXT("%s"), *id);
    //}

    for (const auto& id : PlayersInfo) {
        UE_LOG(LogTemp, Display, TEXT("%s"), *id.participating_user_id);
    }

    /* DEV_TimerTournament
    auto it = tournament_->userid_to_pc.Find(user_id);
    if (it && IsValid(*it)) {
        UE_LOG(LogTemp, Display, TEXT("User with id: %s in match with id: %d has valid PlayerController"), *user_id, match_id_);
        //ready_user_id[participating_user_id.Num() - 1] = true;
        PlayersInfo.Last().accepted_user_id = true;
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("User with id: %s in match with id: %d has not valid PlayerController"), *user_id, match_id_);
    }
    */

    if (PlayersInfo.Num() == 1) {
        GetWorld()->GetTimerManager().SetTimer(FirstPlayerToConnectTimer,this, &UTournamentMatch::FinishFirstPlayerToConnect, 120.0f, false, 0.0f);
    }
    else {
        GetWorld()->GetTimerManager().SetTimer(SecondPlayerToConnectTimer,this, &UTournamentMatch::FinishSecondPlayerToConnect, 120.0f, false, 0.0f);
    }
	return true;
}

void UTournamentMatch::FinishFirstPlayerToConnect()
{
    UE_LOG(LogTemp, Display, TEXT("Finish FirstPlayerToConnectTimer to user ids: %s"), *PlayersInfo[0].participating_user_id);
    if (PlayersInfo[0].ready_user_id == true) {
        return;
    }
    UE_LOG(LogTemp, Display, TEXT("User ids: %s not connect to math with id: %d"), *PlayersInfo[0].participating_user_id, match_id_);
}

void UTournamentMatch::FinishSecondPlayerToConnect()
{
    UE_LOG(LogTemp, Display, TEXT("Finish SecondPlayerToConnectTimer to user ids: %s"), *PlayersInfo[1].participating_user_id);
    if (PlayersInfo[1].ready_user_id == true) {
        return;
    }
    UE_LOG(LogTemp, Display, TEXT("User ids: %s not connect to math with id: %d"), *PlayersInfo[1].participating_user_id, match_id_);
}

bool UTournamentMatch::IsReady() {
    //return participating_user_id.Num() == 2;
    return PlayersInfo.Num() == 2;
}

void UTournamentMatch::RegisterIncomingPlayer(APlayerController* pc) {
    FString user_id = Cast<ATournamentPlayerState>(pc->PlayerState)->user_id;
    int userid_index = -1;

    //for (int i = 0; i < participating_user_id.Num(); ++i) {
    //    if (participating_user_id[i] == user_id)
    //        userid_index = i;
    //}

    for (int i = 0; i < PlayersInfo.Num(); ++i) {
        if (PlayersInfo[i].participating_user_id == user_id)
            userid_index = i;
    }

    if (userid_index == -1) {
        UE_LOG(LogTemp, Error, TEXT("Player with id %s wasn't yet registred in math with id: %s"), *user_id, match_id_);
        return;
    }
    UE_LOG(LogTemp, Display, TEXT("User with id %s is ready in match with id %d. ready_user_id index is %d"), *user_id, match_id_, userid_index);
    //ready_user_id[userid_index] = true;
    PlayersInfo[userid_index].ready_user_id = true;

    if(userid_index > 1){
        UE_LOG(LogTemp, Warning, TEXT("userid_index is %d"), userid_index);
    }

    if (userid_index == 0) {
        GetWorld()->GetTimerManager().ClearTimer(FirstPlayerToConnectTimer);
        UE_LOG(LogTemp, Display, TEXT("Clear FirstPlayerToConnectTimer")); }
    else { 
        GetWorld()->GetTimerManager().ClearTimer(SecondPlayerToConnectTimer);
        UE_LOG(LogTemp, Display, TEXT("Clear SecondPlayerToConnectTimer")); }
}

bool UTournamentMatch::StartIfReady() {
    ///if (ready_user_id.Num() != 2) {
    if (PlayersInfo.Num() != 2) {
        UE_LOG(LogTemp, Error, TEXT("ready_user_id array has size %d, it has to be 2"), PlayersInfo.Num());
        return false;
    }
    //if (!ready_user_id[0] || !ready_user_id[1]) {
    if (!PlayersInfo[0].ready_user_id || !(PlayersInfo[1].ready_user_id)) {
        UE_LOG(LogTemp, Error, TEXT("Can't start match not all players are ready."));
        PlayersInfo[0].ready_user_id = false;
        PlayersInfo[1].ready_user_id = false;
        return false;
    }
    //UE_LOG(LogTemp, Display, TEXT("Sending request to  start match with id: %d and players id: %s, %s"), match_id_, *participating_user_id[0], *participating_user_id[1]);
    UE_LOG(LogTemp, Display, TEXT("Sending request to  start match with id: %d and players id: %s, %s"), match_id_, *PlayersInfo[0].participating_user_id, *PlayersInfo[1].participating_user_id);
   
    cool_json_t json;
    //json["user1_id"] = participating_user_id[0];
    //json["user2_id"] = participating_user_id[1];
    json["user1_id"] = PlayersInfo[0].participating_user_id;
    json["user2_id"] = PlayersInfo[1].participating_user_id;
    json["type"] = static_cast<int>(EPoolGameType::TOURN_GAME_1v1);
    json["params"]["node_id"] = match_id_;
    json["params"]["stage"] = stage_;
    json["params"]["difficulty"] = static_cast<int>(tournament_->difficulty);
    json["params"]["tourn_id"] = tournament_->tourn_id;

    FAllocationRequest req = {};
    req.service = TEXT("core");
    req.endpoint = TEXT("internal/begin_type_1vs1");
    req.json = json;
    
    req.result_callback = [this](FString ip, int port, int match_id) -> void {
        if (GS_IP_FOR_CLIENTS != std::string("NO")) ip = GS_IP_FOR_CLIENTS;
        FName address = FName(ip + ":" + FString::FromInt(port));
        ATournamentPlayerState* ps1 = Cast<ATournamentPlayerState>(tournament_->userid_to_pc[PlayersInfo[0].participating_user_id]->PlayerState);
        ATournamentPlayerState* ps2 = Cast<ATournamentPlayerState>(tournament_->userid_to_pc[PlayersInfo[1].participating_user_id]->PlayerState);
        UE_LOG(LogTemp, Display, TEXT("Succesfuly created gameserver for match_id: %d. Address of gameserver : %s"), match_id_, *address.ToString());
        ps1->TravelToPoolGame(address, ps1->user_id);
        ps2->TravelToPoolGame(address, ps2->user_id);
        tournament_->userid_to_match.Remove(ps1->user_id);
        tournament_->userid_to_match.Remove(ps2->user_id);
        tournament_->userid_to_pc.Remove(ps1->user_id);
        tournament_->userid_to_pc.Remove(ps2->user_id);
    };
    
    tournament_->gs_allocate->RequestAllocation(std::move(req));
    
    ATournamentGameState* gs = GetWorld()->GetGameState<ATournamentGameState>();
    if (!gs) return false;
    
    gs->tournament_info[match_id_].HasStarted();
    
    return true;
}

void UTournamentMatch::BeginDestroy()
{
    Super::BeginDestroy();

    if (FirstPlayerToConnectTimer.IsValid()) {
        GetWorld()->GetTimerManager().ClearTimer(FirstPlayerToConnectTimer);
    }
    if (SecondPlayerToConnectTimer.IsValid()) {
        GetWorld()->GetTimerManager().ClearTimer(SecondPlayerToConnectTimer);
    }
}

void UTournament::Init(ABaseTournamentGameMode* gm, int tourn_id_, EDifficultyLevels difficulty_, int size_) {
    tourn_id = tourn_id_;
    difficulty = difficulty_;
    size = size_;
    gm_ = gm;

#if UE_SERVER
    redis = NewObject<URedisObject>(this);
    redis->AddToRoot();
#endif
    UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!srv_gi) return;
    gs_allocate = NewObject<UGsAsyncQueueAllocator>(this);
    gs_allocate->Init(srv_gi);
    
#if UE_SERVER
    
    if (redis)
    {
        //redis->Init("redis", 6379, "");
        redis->Init(TEXT("127.0.0.1"), 6379, TEXT(""));
        
        redis->AsyncSetInt(TEXT("conn_players"), 0);
        redis->AsyncSetInt(TEXT("available_tickets"), size);
        redis->AsyncSetInt(TEXT("tourn_size"), size);
        
        FScriptDelegate redis_reply_delegat;
        redis_reply_delegat.BindUFunction(this, "ProcessingMatches");
        redis->SubscribeReply.AddUnique(redis_reply_delegat);
        redis->Subscribe(TEXT("game_results"));
        srv_gi->TimerManager->SetTimer(pingTimer, this,
                                       &UTournament::RedisPing, 5.0f, true, 5.0f);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Redis object is invalid."));
    }
#endif
    tournament_end_json["tourn_id"] = tourn_id;
    int tmp_size = size;
    int max_depth = 0;
    tournament_end_json["results"][std::to_string(max_depth)] = cool_json_t::array();
    while(tmp_size > 0)
    {
        tmp_size >>= 1;
        max_depth += 1;
        tournament_end_json["results"][std::to_string(max_depth)] = cool_json_t::array();
    }
    
    tree.Init(nullptr, size - 1);
    int depth = 1;
    for (int i = tree.Num() - 1; i >= 0; i--)
    {
        tree[i] = NewObject<UTournamentMatch>(this);
        tree[i]->AddToRoot();
        tree[i]->Init(this, i, depth);
        if (!(i & (i+1))) depth++; // for each stage the lowest match_id is 2^x - 1
    }
}

void UTournament::RedisPing()
{
#if UE_SERVER
    if (redis)
    {
        redis->Publish(TEXT("game_results"), TEXT("ping"));
    }
#endif
}

void UTournament::BeginDestroy()
{
    Super::BeginDestroy();
    UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!srv_gi) return;
    srv_gi->TimerManager->ClearTimer(pingTimer);
}

bool UTournament::Start()
{
    has_started = true;
    UE_LOG(LogTemp, Display, TEXT("Starting tournament"));
    UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    cool_json_t json = {
        {"tourn_id", tourn_id}
    };

    if (srv_gi)
    {
        srv_gi->MakeRequestWithFail(
            "tournament",
            "free_tourn_start",
            json,
            [this](cool_json_t json) {TRACE("Endpoint free_tourn_start was succesful"); },
            [this]() {TRACE("Endpoint free_tourn_start wasn't succesful"); }
        );
    }
    else
    {
        TRACE("Server gi wasn't valid");
    }
    TArray<FString>  players;
    players.Reserve(userid_to_pc.Num());
    for (const auto& pair : userid_to_pc) {
        players.Add(pair.Key);
    }
    //int64 DateInSeconds = FDateTime::Now().ToUnixTimestamp();
    //FRandomStream SRand = FRandomStream();
    //SRand.Initialize(DateInSeconds);
    //players.Sort([this](APlayerController* Item1, APlayerController* Item2) {
    //    return FMath::FRand() < 0.5f;
    //});

    if (size != players.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("Incorect number of initial players: %d. Note: it has to be %d for tree to be perfect."), players.Num(), size);
        return false;
    }

    //tree.Init(nullptr, size - 1);

    for (int i = tree.Num() - 1, j = 0; j < players.Num(); --i, j += 2)
    {
        UE_LOG(LogTemp, Error, TEXT("Processing initial match with id: %d"), i);
        /*tree[i] = NewObject<UTournamentMatch>(this);
        tree[i]->Init(this, i, 1);
        tree[i]->AddPlayer(players[j]);
        tree[i]->AddPlayer(players[j + 1]);*/
        tree[i]->StartIfReady();
    }

    return true;
}

void UTournament::RegisterIncomingPlayer(APlayerController* pc) {
    
    FString user_id = Cast<ATournamentPlayerState>(pc->PlayerState)->user_id;
    UE_LOG(LogTemp, Display, TEXT("Player with user_id %s accepted to tournament"), *user_id);
    userid_to_pc.Add(user_id, pc);
#if UE_SERVER
    if (redis)
        redis->AsyncSetInt(TEXT("conn_players"), !has_started ? userid_to_pc.Num() : size);
    else
        UE_LOG(LogTemp, Error, TEXT("Redis object is invalid."));
#endif
    

    if (has_started) {
        auto it = userid_to_match.Find(user_id);
        if (it) {
            UTournamentMatch* match = *it;
            if (match) {
                match->RegisterIncomingPlayer(pc);
                match->StartIfReady();
            }
            else {
                UE_LOG(LogTemp, Error, TEXT("match for user_id %s was constructed but is nullptr"), *user_id);
            }
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Match wasn't constructed for user with id: %s\nCurrent userid_to_match"), *user_id);
            for (auto pair : userid_to_match) {
                UE_LOG(LogTemp, Error, TEXT("%s: %d"), *pair.Key, pair.Value->match_id_);
            }
        }
    }
    else
    {
        for (int i = tree.Num() - 1; i >= size / 2 - 1; i--) // only first stage
        {
            if (!tree[i]->IsReady())
            {
                tree[i]->AddPlayer(user_id);
                break;
            }
        }
    }
    if (userid_to_username.Contains(user_id)) return;
    
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!gi) return;
    ATournamentGameState* gs = GetWorld()->GetGameState<ATournamentGameState>();
    if (gs)
        gs->CheckUsernames(userid_to_username);
    if (userid_to_username.Contains(user_id))
        return;
    
    cool_json_t username_req;
    username_req["user_id"] = user_id;
    
    auto callback = [this, user_id](cool_json_t json) -> void {
            UE_LOG(LogTemp, Warning, TEXT("Processing reponse user/user_id_to_username"));
            if (!json.contains("status") || json["status"] != 200)
            {
                UE_LOG(LogTemp, Error, TEXT("Bad status on user/user_id_to_username"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Got username %s"), *json["username"].get<FString>());
                userid_to_username.Add(user_id, json["username"].get<FString>());
                ATournamentGameState* gs = GetWorld()->GetGameState<ATournamentGameState>();
                if (gs)
                    gs->CheckUsernames(userid_to_username);
            }
    };
    
    gi->MakeRequest("user", "user_id_to_username", username_req, callback);
    
    
    if (userid_to_pc.Num() >= size && !has_started)
    {
        has_started = Start();
        return;
    }
}

void UTournament::ProcessingMatches(const FString& channel, const FString& message)
{
    UE_LOG(LogTemp, Warning, TEXT("Get message from redis, message is:\n%s"), *message);
    if (message == TEXT("ping")) return;
    cool_json_t json = cool_json_t::parse(std::string(TCHAR_TO_UTF8(*message)));
    FString win_id = json["win_user_id"].get<FString>();
    FString lose_id = json["lose_user_id"].get<FString>();
    int match_id = json["match_id"].get<int>();
    UE_LOG(LogTemp, Warning, TEXT("Message fields:\nwin_user_id: %s\nlose_user_id: %s\nmatch_id: %d"), *win_id, *lose_id, match_id);
    userid_to_pc.Add(win_id, nullptr);
    int match_depth = 1;
    int tmp_match_id = match_id;
    while (tmp_match_id != 0)
    {
        match_depth++;
        tmp_match_id = (tmp_match_id - 1) / 2;
    }
    tournament_end_json["results"][std::to_string(match_depth)].push_back(lose_id);
    
    
    ATournamentGameState* gs = GetWorld()->GetGameState<ATournamentGameState>();
    if (gs)
        gs->tournament_info[match_id].HasWon(win_id, userid_to_username);
    

    if (match_id == 0) {
        tournament_end_json["results"]["0"].push_back(win_id);
        UE_LOG(LogTemp, Warning, TEXT("Tournament has ended, json: %s"), *(FString(tournament_end_json.dump().c_str())));
        UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
        if (srv_gi)
        {
            UE_LOG(LogTemp, Warning, TEXT("tourn_end json: %s"))
            srv_gi->MakeRequestWithFail(
                "tournament",
                "free_tourn_end",
                tournament_end_json,
                [this](cool_json_t json) {
                    TRACE("Endpoint free_tourn_end was succesful"); 
                    for (auto& win_amount : json["win_amounts"].items())
                    {
                        win_amounts.Add(FCString::Atoi(*FString(win_amount.key().c_str())), win_amount.value().get<int>());
                    }
                    
                },
                [this]() {TRACE("Endpoint free_tourn_end wasn't succesful"); }
            );
        }
        else
        {
            TRACE("Server gi wasn't valid");
        }
        has_ended = true;
    }
    else {
        int match_id_parent = (match_id - 1) / 2;
        if (tree[match_id_parent] == nullptr) {
            TRACE("Match with id %d is nullptr",match_id_parent);
            //tree[match_id_parent] = NewObject<UTournamentMatch>(this);
            //tree[match_id_parent]->Init(this, match_id_parent, 2);
        }
        TRACE("Match with id %d is not nullptr", match_id_parent);
        tree[match_id_parent]->AddPlayer(win_id);
        tree[match_id_parent]->StartIfReady();
    }
}
