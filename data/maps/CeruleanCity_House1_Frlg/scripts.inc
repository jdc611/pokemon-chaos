CeruleanCity_House1_Frlg_MapScripts::
	.byte 0

CeruleanCity_House1_EventScript_BadgeGuy::
	lock
	faceplayer
	goto_if_eq VAR_CHAOS_CHANGERS_UNLOCKED, 1, CeruleanCity_House1_EventScript_QuizAlreadyPassed
	msgbox CeruleanCity_House1_Text_QuizOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CeruleanCity_House1_EventScript_QuizDeclined
	setvar VAR_0x8004, 0
CeruleanCity_House1_EventScript_QuizQuestion::
	callnative ChaosQuizPrepareQuestion
	message gStringVar4
	waitmessage
	multichoice 14, 0, MULTI_CHAOS_KNOWLEDGE_QUIZ, FALSE
	goto_if_eq VAR_RESULT, 127, CeruleanCity_House1_EventScript_QuizDeclined
	goto_if_ne VAR_RESULT, VAR_0x8005, CeruleanCity_House1_EventScript_QuizFailed
	addvar VAR_0x8004, 1
	goto_if_lt VAR_0x8004, 3, CeruleanCity_House1_EventScript_QuizNextQuestion
	setvar VAR_CHAOS_CHANGERS_UNLOCKED, 1
	msgbox CeruleanCity_House1_Text_QuizReward
	release
	end

CeruleanCity_House1_EventScript_QuizNextQuestion::
	msgbox CeruleanCity_House1_Text_QuizCorrect
	goto CeruleanCity_House1_EventScript_QuizQuestion

CeruleanCity_House1_EventScript_QuizFailed::
	msgbox CeruleanCity_House1_Text_QuizTryAgain
	release
	end

CeruleanCity_House1_EventScript_QuizDeclined::
	msgbox CeruleanCity_House1_Text_QuizComeBack
	release
	end

CeruleanCity_House1_EventScript_QuizAlreadyPassed::
	msgbox CeruleanCity_House1_Text_QuizAlreadyPassed
	release
	end

CeruleanCity_House1_EventScript_DescribeAnotherBadge::
	message CeruleanCity_House1_Text_DescribeWhichBadge
	waitmessage
	setvar VAR_0x8004, SCROLL_MULTI_BADGES
	special ShowScrollableMultichoice
	switch VAR_RESULT
	case 0, CeruleanCity_House1_EventScript_DescribeBoulderBadge
	case 1, CeruleanCity_House1_EventScript_DescribeCascadeBadge
	case 2, CeruleanCity_House1_EventScript_DescribeThunderBadge
	case 3, CeruleanCity_House1_EventScript_DescribeRainbowBadge
	case 4, CeruleanCity_House1_EventScript_DescribeSoulBadge
	case 5, CeruleanCity_House1_EventScript_DescribeMarshBadge
	case 6, CeruleanCity_House1_EventScript_DescribeVolcanoBadge
	case 7, CeruleanCity_House1_EventScript_DescribeEarthBadge
	case 8, CeruleanCity_House1_EventScript_StopDescribingBadges
	case 127, CeruleanCity_House1_EventScript_StopDescribingBadges
	end

CeruleanCity_House1_EventScript_DescribeBoulderBadge::
	msgbox CeruleanCity_House1_Text_AttackStatFlash
	goto CeruleanCity_House1_EventScript_DescribeAnotherBadge
	end

CeruleanCity_House1_EventScript_DescribeCascadeBadge::
	msgbox CeruleanCity_House1_Text_ObeyLv30Cut
	goto CeruleanCity_House1_EventScript_DescribeAnotherBadge
	end

CeruleanCity_House1_EventScript_DescribeThunderBadge::
	msgbox CeruleanCity_House1_Text_SpeedStatFly
	goto CeruleanCity_House1_EventScript_DescribeAnotherBadge
	end

CeruleanCity_House1_EventScript_DescribeRainbowBadge::
	msgbox CeruleanCity_House1_Text_ObeyLv50Strength
	goto CeruleanCity_House1_EventScript_DescribeAnotherBadge
	end

CeruleanCity_House1_EventScript_DescribeSoulBadge::
	msgbox CeruleanCity_House1_Text_DefenseStatSurf
	goto CeruleanCity_House1_EventScript_DescribeAnotherBadge
	end

CeruleanCity_House1_EventScript_DescribeMarshBadge::
	msgbox CeruleanCity_House1_Text_ObeyLv70RockSmash
	goto CeruleanCity_House1_EventScript_DescribeAnotherBadge
	end

CeruleanCity_House1_EventScript_DescribeVolcanoBadge::
	msgbox CeruleanCity_House1_Text_SpStatsWaterfall
	goto CeruleanCity_House1_EventScript_DescribeAnotherBadge
	end

CeruleanCity_House1_EventScript_DescribeEarthBadge::
	msgbox CeruleanCity_House1_Text_AllMonsWillObeyYou
	goto CeruleanCity_House1_EventScript_DescribeAnotherBadge
	end

CeruleanCity_House1_EventScript_StopDescribingBadges::
	msgbox CeruleanCity_House1_Text_ComeVisitAnytime
	release
	end

CeruleanCity_House1_Text_BadgesHaveAmazingSecrets::
	.string "Only skilled TRAINERS can collect\n"
	.string "POKéMON BADGES.\p"
	.string "I see you have at least one.\p"
	.string "Those BADGES have amazing secrets,\n"
	.string "did you know?$"

CeruleanCity_House1_Text_DescribeWhichBadge::
	.string "Now then…\p"
	.string "Which of the eight BADGES should\n"
	.string "I describe?$"

CeruleanCity_House1_Text_ComeVisitAnytime::
	.string "Come visit me anytime you wish.$"

CeruleanCity_House1_Text_AttackStatFlash::
	.string "The ATTACK stat of all your\n"
	.string "POKéMON increases a little bit.\p"
	.string "It also lets you use the move\n"
	.string "FLASH outside of battle.$"

CeruleanCity_House1_Text_ObeyLv30Cut::
	.string "POKéMON up to Lv. 30 will obey\n"
	.string "you.\p"
	.string "That includes even outsiders you\n"
	.string "obtain in trades.\p"
	.string "POKéMON of higher levels will\n"
	.string "be unruly in battle, however.\p"
	.string "It also lets you use the move\n"
	.string "CUT outside of battle.$"

CeruleanCity_House1_Text_SpeedStatFly::
	.string "The SPEED stat of all your\n"
	.string "POKéMON increases a little bit.\p"
	.string "It also lets you use the move\n"
	.string "FLY outside of battle.$"

CeruleanCity_House1_Text_ObeyLv50Strength::
	.string "POKéMON up to Lv. 50 will obey\n"
	.string "you.\p"
	.string "That includes even outsiders you\n"
	.string "obtain in trades.\p"
	.string "POKéMON of higher levels will\n"
	.string "be unruly in battle, however.\p"
	.string "It also lets you use the move\n"
	.string "STRENGTH outside of battle.$"

CeruleanCity_House1_Text_DefenseStatSurf::
	.string "The DEFENSE stat of all your\n"
	.string "POKéMON increases a little bit.\p"
	.string "It also lets you use the move\n"
	.string "SURF outside of battle.$"

CeruleanCity_House1_Text_ObeyLv70RockSmash::
	.string "POKéMON up to Lv. 70 will obey\n"
	.string "you.\p"
	.string "That includes even outsiders you\n"
	.string "obtain in trades.\p"
	.string "POKéMON of higher levels will\n"
	.string "be unruly in battle, however.\p"
	.string "It also lets you use the move\n"
	.string "ROCK SMASH outside of battle.$"

CeruleanCity_House1_Text_SpStatsWaterfall::
	.string "The SP. ATK and SP. DEF stats of\n"
	.string "all your POKéMON rise a little bit.\p"
	.string "It also lets you use the move\n"
	.string "WATERFALL outside of battle.$"

CeruleanCity_House1_Text_AllMonsWillObeyYou::
	.string "All POKéMON will obey you!$"


CeruleanCity_House1_Text_QuizOffer::
	.string "I can teach your POKéMON some new\n"
	.string "tricks! First, let's test your\l"
	.string "POKéMON knowledge.\p"
	.string "Answer three questions correctly,\n"
	.string "and I'll share my special tools!\p"
	.string "Shall we begin?$"

CeruleanCity_House1_Text_QuizCorrect::
	.string "That's right!\n"
	.string "Let's try another one.$"

CeruleanCity_House1_Text_QuizTryAgain::
	.string "Not quite! Even great TRAINERS\n"
	.string "have more to learn.\p"
	.string "Come try again whenever you like!$"

CeruleanCity_House1_Text_QuizComeBack::
	.string "No rush! Come back when you're\n"
	.string "ready to try some new tricks!$"

CeruleanCity_House1_Text_QuizReward::
	.string "Excellent! You really know your\n"
	.string "POKéMON! Here are my special tools.\p"
	.string "You can now change your POKéMON's\n"
	.string "NATURE, ABILITY, and GENDER!\p"
	.string "Open your quick tools with L,\n"
	.string "then turn to the second page.$"

CeruleanCity_House1_Text_QuizAlreadyPassed::
	.string "How are those new tricks working?\p"
	.string "Your NATURE, ABILITY, and GENDER\n"
	.string "tools are on quick tools page two.$"
