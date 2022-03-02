#include <Arduino.h>
#define MAX_BRIGHTNESS 255
#define NUM_LEDS 25
#define SLACK_TOKEN ""

const String reaction_list[8][32] = {
	{"pray", "joy", "heart", "+1", "kissing_heart", "smiling_face_with_3_hearts", "smile", "smiley", "blush", "rolling_on_the_floor_laughing", "pleading_face", "raised_hands", "clap", "innocent"}, // 平穏・喜び・恍惚
	{"pray", "heart", "+1", "smiling_face_with_3_hearts", "heart_eyes", "rolling_on_the_floor_laughing", "pleading_face", "clap"}, // 容認・信頼・敬愛
	{"joy", "sob", "pleading_face", "persevere", "tired_face"}, // 不安・恐れ・恐怖
	{"joy", "sob", "rolling_on_the_floor_laughing", "tired_face", "innocent"}, // 放心・驚き・驚嘆
	{"joy", "sob", "persevere"}, // 哀愁・悲しみ・悲嘆
	{"persevere", "tired_face"}, // うんざり・嫌悪・強い嫌悪
	{"muscle", "rage", "santa"}, // 苛立ち・怒り・激怒
	{"pray", "+1", "kissing_heart", "smiling_face_with_3_hearts", "heart_eyes", "pleading_face", "clap", "muscle"} // 関心・期待・警戒
};