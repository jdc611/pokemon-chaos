CeruleanCity_BikeShop_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, CeruleanCity_BikeShop_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CeruleanCity_BikeShop_OnFrame
	.byte 0

CeruleanCity_BikeShop_OnTransition::
	setvar VAR_TEMP_1, 0
	checkitem ITEM_BICYCLE
	goto_if_eq VAR_RESULT, FALSE, CeruleanCity_BikeShop_EventScript_ReadyGift
	setflag FLAG_GOT_BICYCLE
	setvar VAR_TEMP_1, 1
CeruleanCity_BikeShop_EventScript_ReadyGift::
	end

CeruleanCity_BikeShop_OnFrame::
	map_script_2 VAR_TEMP_1, 0, CeruleanCity_BikeShop_EventScript_EntryGift
	.2byte 0

CeruleanCity_BikeShop_EventScript_EntryGift::
	lockall
	turnobject 1, DIR_SOUTH
	goto CeruleanCity_BikeShop_EventScript_FreeBicycle

CeruleanCity_BikeShop_EventScript_Clerk::
	lockall
	faceplayer
	checkitem ITEM_BICYCLE
	goto_if_eq VAR_RESULT, TRUE, CeruleanCity_BikeShop_EventScript_AlreadyGotBicycle
	goto CeruleanCity_BikeShop_EventScript_FreeBicycle

CeruleanCity_BikeShop_EventScript_FreeBicycle::
	setvar VAR_TEMP_1, 1
	msgbox CeruleanCity_BikeShop_Text_MillionthCustomer
	additem ITEM_BICYCLE
	goto_if_eq VAR_RESULT, FALSE, CeruleanCity_BikeShop_EventScript_NoRoomForBicycle
	setflag FLAG_GOT_BICYCLE
	checkitem ITEM_BIKE_VOUCHER
	goto_if_eq VAR_RESULT, FALSE, CeruleanCity_BikeShop_EventScript_ReceiveFreeBicycle
	removeitem ITEM_BIKE_VOUCHER
CeruleanCity_BikeShop_EventScript_ReceiveFreeBicycle::
	msgreceiveditem CeruleanCity_BikeShop_Text_ReceivedFreeBicycle, ITEM_BICYCLE, 1, MUS_RG_OBTAIN_KEY_ITEM
	msgbox CeruleanCity_BikeShop_Text_FreeRide
	releaseall
	end

CeruleanCity_BikeShop_EventScript_AlreadyGotBicycle::
	setflag FLAG_GOT_BICYCLE
	msgbox CeruleanCity_BikeShop_Text_HowDoYouLikeNewBicycle
	releaseall
	end

CeruleanCity_BikeShop_EventScript_NoRoomForBicycle::
	msgbox CeruleanCity_BikeShop_Text_MakeRoomForBicycle
	releaseall
	end

CeruleanCity_BikeShop_EventScript_Woman::
	msgbox CeruleanCity_BikeShop_Text_CityBikeGoodEnoughForMe, MSGBOX_NPC
	end

CeruleanCity_BikeShop_EventScript_Youngster::
	lock
	faceplayer
	goto_if_set FLAG_GOT_BICYCLE, CeruleanCity_BikeShop_EventScript_YoungsterHaveBike
	msgbox CeruleanCity_BikeShop_Text_BikesCoolButExpensive
	release
	end

CeruleanCity_BikeShop_EventScript_YoungsterHaveBike::
	msgbox CeruleanCity_BikeShop_Text_WowYourBikeIsCool
	release
	end

CeruleanCity_BikeShop_EventScript_Bicycle::
	msgbox CeruleanCity_BikeShop_Text_ShinyNewBicycle, MSGBOX_SIGN
	end

CeruleanCity_BikeShop_Text_WelcomeToBikeShop::
	.string "Hi!\n"
	.string "Welcome to our BIKE SHOP.\p"
	.string "Have we got just the BIKE for\n"
	.string "you!$"

CeruleanCity_BikeShop_Text_SorryYouCantAffordIt::
	.string "Sorry!\n"
	.string "You can't afford it!$"

CeruleanCity_BikeShop_Text_OhBikeVoucherHereYouGo::
	.string "Oh, that's…\p"
	.string "A BIKE VOUCHER!\p"
	.string "Okay!\n"
	.string "Here you go!$"

CeruleanCity_BikeShop_Text_ExchangedVoucherForBicycle::
	.string "{PLAYER} exchanged the\n"
	.string "BIKE VOUCHER for a BICYCLE.$"

CeruleanCity_BikeShop_Text_ThankYouComeAgain::
	.string "Thank you!\n"
	.string "Come back again sometime!$"

CeruleanCity_BikeShop_Text_HowDoYouLikeNewBicycle::
	.string "How do you like your new BICYCLE?\n"
	.string "Do you like how it rides?\p"
	.string "You can take it out on CYCLING\n"
	.string "ROAD and even into caves!$"

CeruleanCity_BikeShop_Text_MakeRoomForBicycle::
	.string "You better make room for your\n"
	.string "BICYCLE!$"

CeruleanCity_BikeShop_Text_CityBikeGoodEnoughForMe::
	.string "A plain city bike is good enough\n"
	.string "for me.\p"
	.string "After all, you can't put a shopping\n"
	.string "basket on a mountain bike.$"

CeruleanCity_BikeShop_Text_BikesCoolButExpensive::
	.string "These bikes are cool! I wonder\n"
	.string "who will win the shop's big prize?$"

CeruleanCity_BikeShop_Text_WowYourBikeIsCool::
	.string "Wow.\n"
	.string "Your BIKE is really cool!$"

@ Unused
CeruleanCity_BikeShop_Text_GermanFoldableBicyleFinallyOnMarket::
	.string "ついに　はつばい！\p"
	.string "ドイツ　せい　さいこうきゅう\n"
	.string "おりたたみ　じてんしゃ！$"

CeruleanCity_BikeShop_Text_ShinyNewBicycle::
	.string "A shiny new BICYCLE!$"


CeruleanCity_BikeShop_Text_MillionthCustomer::
	.string "Wait! You're our one-millionth\n"
	.string "customer! What incredible timing!\p"
	.string "Your prize is a brand-new BICYCLE!\n"
	.string "No voucher, no charge!$"

CeruleanCity_BikeShop_Text_ReceivedFreeBicycle::
	.string "{PLAYER} received a BICYCLE!$"

CeruleanCity_BikeShop_Text_FreeRide::
	.string "Go on, take it for a spin!\n"
	.string "Just watch out for the flowerbeds!$"
