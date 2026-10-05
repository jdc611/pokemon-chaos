CeladonCity_GameCorner_PrizeRoom_Frlg_MapScripts::
	.byte 0

CeladonCity_GameCorner_PrizeRoom_EventScript_BaldingMan::
	msgbox CeladonCity_GameCorner_PrizeRoom_Text_FancyThatPorygon, MSGBOX_NPC
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_OldMan::
	msgbox CeladonCity_GameCorner_PrizeRoom_Text_RakedItInToday, MSGBOX_NPC
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_PrizeClerkMons::
	lock
	faceplayer
	goto_if_unset FLAG_BADGE04_GET, ChaosArcadePrize_Construction
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, CeladonCity_GameCorner_PrizeRoom_EventScript_NeedCoinCase
	showcoinsbox 20, 0
	msgbox ChaosArcadePrize_Welcome
	closemessage
	special ChaosArcadePrizeMenu
	goto_if_eq VAR_RESULT, 127, ChaosArcadePrize_Cancel
	special ChaosArcadePrizePick
	goto_if_eq VAR_RESULT, 2, ChaosArcadePrize_Cancel
	goto_if_eq VAR_RESULT, 1, ChaosArcadePrize_Rotom
	goto ChaosArcadePrize_Tier

ChaosArcadePrize_Rotom::
	special ChaosArcadeRotomMenu
	goto_if_eq VAR_RESULT, 127, ChaosArcadePrize_Cancel
	special ChaosArcadeRotomPick
	goto_if_eq VAR_RESULT, 2, ChaosArcadePrize_Cancel

ChaosArcadePrize_Tier::
	special ChaosArcadePrizeHasHidden
	goto_if_eq VAR_RESULT, TRUE, ChaosArcadePrize_TierMenu
	msgbox ChaosArcadePrize_NoHiddenText
	closemessage

ChaosArcadePrize_TierMenu::
	special ChaosArcadeTierMenu
	goto_if_eq VAR_RESULT, 127, ChaosArcadePrize_Cancel
	special ChaosArcadePrizePrepare
	goto_if_eq VAR_RESULT, 2, ChaosArcadePrize_Cancel
	goto_if_eq VAR_RESULT, 1, ChaosArcadePrize_FilterWarning
	goto ChaosArcadePrize_Confirm

ChaosArcadePrize_FilterWarning::
	msgbox ChaosArcadePrize_FilterText

ChaosArcadePrize_Confirm::
	msgbox ChaosArcadePrize_Offer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ChaosArcadePrize_Cancel
	special ChaosArcadePrizeBuy
	updatecoinsbox
	goto_if_eq VAR_RESULT, 0, ChaosArcadePrize_Party
	goto_if_eq VAR_RESULT, 1, ChaosArcadePrize_PC
	goto_if_eq VAR_RESULT, 2, CeladonCity_GameCorner_PrizeRoom_EventScript_NotEnoughCoins
	goto_if_eq VAR_RESULT, 3, ChaosArcadePrize_Full
	goto ChaosArcadePrize_Cancel

ChaosArcadePrize_Party::
	playfanfare MUS_LEVEL_UP
	msgbox ChaosArcadePrize_ReceivedParty
	waitfanfare
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange

ChaosArcadePrize_PC::
	playfanfare MUS_LEVEL_UP
	msgbox ChaosArcadePrize_ReceivedPC
	waitfanfare
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange

ChaosArcadePrize_Full::
	msgbox gText_NoMoreRoomForPokemon
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange

ChaosArcadePrize_Cancel::
	special ChaosArcadePrizeCancel
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange

ChaosArcadePrize_Construction::
	msgbox ChaosArcadePrize_ConstructionText
	release
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange::
	hidecoinsbox
	release
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_NeedCoinCase::
	textcolor NPC_TEXT_COLOR_NEUTRAL
	msgbox CeladonCity_GameCorner_PrizeRoom_Text_CoinCaseRequired
	release
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_NotEnoughCoins::
	msgbox CeladonCity_GameCorner_PrizeRoom_Text_NeedMoreCoins
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_PrizeClerkTMs::
	goto ChaosArcadeItemCounter
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_ChoosePrizeTM::
	message CeladonCity_GameCorner_PrizeRoom_Text_WhichPrize
	waitmessage
	multichoice 11, 0, MULTI_GAME_CORNER_TMPRIZES, FALSE
	switch VAR_RESULT
	case 0, CeladonCity_GameCorner_PrizeRoom_EventScript_TM13
	case 1, CeladonCity_GameCorner_PrizeRoom_EventScript_TM23
	case 2, CeladonCity_GameCorner_PrizeRoom_EventScript_TM24
	case 3, CeladonCity_GameCorner_PrizeRoom_EventScript_TM30
	case 4, CeladonCity_GameCorner_PrizeRoom_EventScript_TM35
	case 5, CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	case 127, CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_TM13::
	setvar VAR_TEMP_1, ITEM_TM13
	setvar VAR_TEMP_2, 4000
	buffermovename STR_VAR_2, MOVE_ICE_BEAM
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeTM
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_TM23::
	setvar VAR_TEMP_1, ITEM_TM23
	setvar VAR_TEMP_2, 3500
	buffermovename STR_VAR_2, MOVE_IRON_TAIL
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeTM
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_TM24::
	setvar VAR_TEMP_1, ITEM_TM24
	setvar VAR_TEMP_2, 4000
	buffermovename STR_VAR_2, MOVE_THUNDERBOLT
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeTM
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_TM30::
	setvar VAR_TEMP_1, ITEM_TM30
	setvar VAR_TEMP_2, 4500
	buffermovename STR_VAR_2, MOVE_SHADOW_BALL
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeTM
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_TM35::
	setvar VAR_TEMP_1, ITEM_TM35
	setvar VAR_TEMP_2, 4000
	buffermovename STR_VAR_2, MOVE_FLAMETHROWER
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeTM
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeTM::
	msgbox CeladonCity_GameCorner_PrizeRoom_Text_YouWantTM, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_TryGivePrize
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeItem::
	bufferitemname STR_VAR_1, VAR_TEMP_1
	msgbox CeladonCity_GameCorner_PrizeRoom_Text_YouWantPrize, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_TryGivePrize
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_TryGivePrize::
	bufferitemname STR_VAR_1, VAR_TEMP_1
	checkcoins VAR_RESULT
	goto_if_lt VAR_RESULT, VAR_TEMP_2, CeladonCity_GameCorner_PrizeRoom_EventScript_NotEnoughCoins
	checkitemspace VAR_TEMP_1
	goto_if_eq VAR_RESULT, FALSE, CeladonCity_GameCorner_PrizeRoom_EventScript_BagFull
	removecoins VAR_TEMP_2
	updatecoinsbox
	giveitem VAR_TEMP_1
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_BagFull::
	textcolor NPC_TEXT_COLOR_NEUTRAL
	msgbox gText_TooBadBagIsFull
	hidecoinsbox
	release
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_PrizeClerkItems::
	goto ChaosArcadeItemCounter
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_ChoosePrizeItem::
	message CeladonCity_GameCorner_PrizeRoom_Text_WhichPrize
	waitmessage
	multichoice 10, 0, MULTI_GAME_CORNER_BATTLE_ITEM_PRIZES, FALSE
	switch VAR_RESULT
	case 0, CeladonCity_GameCorner_PrizeRoom_EventScript_SmokeBall
	case 1, CeladonCity_GameCorner_PrizeRoom_EventScript_MiracleSeed
	case 2, CeladonCity_GameCorner_PrizeRoom_EventScript_Charcoal
	case 3, CeladonCity_GameCorner_PrizeRoom_EventScript_MysticWater
	case 4, CeladonCity_GameCorner_PrizeRoom_EventScript_YellowFlute
	case 5, CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	case 127, CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_SmokeBall::
	setvar VAR_TEMP_1, ITEM_SMOKE_BALL
	setvar VAR_TEMP_2, 800
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeItem
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_MiracleSeed::
	setvar VAR_TEMP_1, ITEM_MIRACLE_SEED
	setvar VAR_TEMP_2, 1000
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeItem
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_Charcoal::
	setvar VAR_TEMP_1, ITEM_CHARCOAL
	setvar VAR_TEMP_2, 1000
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeItem
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_MysticWater::
	setvar VAR_TEMP_1, ITEM_MYSTIC_WATER
	setvar VAR_TEMP_2, 1000
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeItem
	end

CeladonCity_GameCorner_PrizeRoom_EventScript_YellowFlute::
	setvar VAR_TEMP_1, ITEM_YELLOW_FLUTE
	setvar VAR_TEMP_2, 1600
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeItem
	end

CeladonCity_GameCorner_PrizeRoom_Text_FancyThatPorygon::
	.string "I sure do fancy that PORYGON!\n"
	.string "But, it's hard to win at slots!$"

CeladonCity_GameCorner_PrizeRoom_Text_RakedItInToday::
	.string "Gahaha! I raked it in today!\n"
	.string "If only every day were like this…$"

CeladonCity_GameCorner_PrizeRoom_Text_CoinCaseRequired::
	.string "A COIN CASE is required…$"

CeladonCity_GameCorner_PrizeRoom_Text_WeExchangeCoinsForPrizes::
	.string "We exchange your COINS for prizes.$"

CeladonCity_GameCorner_PrizeRoom_Text_WhichPrize::
	.string "Which prize would you like?$"

CeladonCity_GameCorner_PrizeRoom_Text_HereYouGo::
	.string "はい どうぞ$"

CeladonCity_GameCorner_PrizeRoom_Text_YouWantPrize::
	.string "So, you want the {STR_VAR_1}?$"

CeladonCity_GameCorner_PrizeRoom_Text_YouWantTM::
	.string "Okay, a TM of {STR_VAR_2}\n"
	.string "is what you want?$"

CeladonCity_GameCorner_PrizeRoom_Text_NeedMoreCoins::
	.string "Sorry, you'll need more COINS\n"
	.string "than that.$"

CeladonCity_GameCorner_PrizeRoom_Text_OopsNotEnoughRoom::
	.string "おきゃくさん もう もてないよ$"

CeladonCity_GameCorner_PrizeRoom_Text_OhFineThen::
	.string "あっ そう$"


ChaosArcadePrize_Welcome:
	.string "Welcome to the Pokémon prize counter!\n"
	.string "Choose a partner, then a prize tier.\p"
	.string "All our partners arrive at Lv. 20.\n"
	.string "The menu shows the full Coin price.$"

ChaosArcadePrize_Offer:
	.string "{STR_VAR_1} - Lv. 20\n"
	.string "ABILITY: {STR_VAR_2}\p"
	.string "{STR_VAR_3}\n"
	.string "Exchange your Coins for this prize?$"

ChaosArcadePrize_FilterText:
	.string "This Pokémon doesn't meet your run's\n"
	.string "current filter.\p"
	.string "Normal party restrictions still apply.\n"
	.string "You can keep it in PC Storage.$"

ChaosArcadePrize_ReceivedParty:
	.string "{PLAYER} received {STR_VAR_1}!\n"
	.string "Your new partner joined your party.$"

ChaosArcadePrize_ReceivedPC:
	.string "{PLAYER} received {STR_VAR_1}!\n"
	.string "Your new partner went to PC Storage.$"

ChaosArcadePrize_ConstructionText:
	.string "We're under construction! Exciting\n"
	.string "new games and prizes are coming.\p"
	.string "Come back with ERIKA's RAINBOW BADGE!$"

ChaosArcadePrize_NoHiddenText:
	.string "This species has no separate Hidden\n"
	.string "Ability. Choose Normal or Shiny.$"

ChaosArcadeItemCounter::
	lock
	faceplayer
	goto_if_unset FLAG_BADGE04_GET, ChaosArcadePrize_Construction
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, CeladonCity_GameCorner_PrizeRoom_EventScript_NeedCoinCase
	showcoinsbox 20, 0
	msgbox ChaosArcadeItem_Welcome
ChaosArcadeItem_Category::
	closemessage
	special ChaosArcadeItemCategories
	goto_if_eq VAR_RESULT, 127, CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	special ChaosArcadeItemMenu
	goto_if_eq VAR_RESULT, 127, CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	special ChaosArcadeItemPreview
	goto_if_eq VAR_RESULT, 2, CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
	msgbox gStringVar4
	msgbox ChaosArcadeItem_Confirm, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ChaosArcadeItem_Category
	special ChaosArcadeItemBuy
	updatecoinsbox
	switch VAR_RESULT
	case 0, ChaosArcadeItem_Received
	case 2, CeladonCity_GameCorner_PrizeRoom_EventScript_NotEnoughCoins
	case 3, ChaosArcadeItem_NoSpace
	case 5, ChaosArcadeItem_Owned
	goto CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange
ChaosArcadeItem_Received::
	playse SE_SHOP
	msgbox ChaosArcadeItem_ReceivedText
	goto ChaosArcadeItem_Category
ChaosArcadeItem_NoSpace::
	msgbox ChaosArcadeItem_NoSpaceText
	goto ChaosArcadeItem_Category
ChaosArcadeItem_Owned::
	msgbox ChaosArcadeItem_OwnedText
	goto ChaosArcadeItem_Category
ChaosArcadeItem_Welcome:
	.string "Our TMs are reusable! Pick a move\n"
	.string "to see its details before buying.\p"
	.string "We also stock useful battle items.$"
ChaosArcadeItem_Confirm:
	.string "{STR_VAR_1}: {STR_VAR_2} COINS\n"
	.string "Exchange Coins for this prize?$"
ChaosArcadeItem_ReceivedText:
	.string "{PLAYER} received {STR_VAR_1}!$"
ChaosArcadeItem_NoSpaceText:
	.string "Your Bag is full. No Coins were used.$"
ChaosArcadeItem_OwnedText:
	.string "You already own this reusable TM!\n"
	.string "No Coins were used.$"
