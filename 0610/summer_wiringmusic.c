#include <stdio.h>
#include <wiringPi.h>
#include <softTone.h>

// 부저가 연결된 GPIO 핀 번호 (wiringPi GPIO 번호 사용)
#define BUZZER_PIN 6 // 예시: wiringPi GPIO 1번 (BCM 18)

// --- 음계 주파수 정의 (C4 = 미들 C) ---
// 옥타브 3 (낮은 음)
#define C3  131
#define C3s 139
#define D3  147
#define D3s 156
#define E3  165
#define F3  175
#define F3s 185
#define G3  196
#define G3s 208
#define A3  220
#define A3s 233
#define B3  247

// 옥타브 4 (중간 음)
#define C4  262
#define C4s 277
#define D4  294
#define D4s 311
#define E4  330
#define F4  349
#define F4s 370
#define G4  392
#define G4s 415
#define A4  440
#define A4s 466
#define B4  494

// 옥타브 5 (높은 음)
#define C5  523
#define C5s 554
#define D5  587
#define D5s 622
#define E5  659
#define F5  698
#define F5s 740
#define G5  784
#define G5s 830
#define A5  880
#define A5s 923
#define B5  988

#define REST 0 // 쉼표

// --- 박자 관련 정의 (템포 120 BPM 기준) ---
// 템포 120 BPM -> 1박 (4분음표)은 0.5초 = 500ms
#define QUARTER_NOTE_MS 500 // 4분음표의 기본 길이 (밀리초)

// 다른 음표들의 길이 계산
#define WHOLE_NOTE_MS   (QUARTER_NOTE_MS * 4)   // 온음표
#define HALF_NOTE_MS    (QUARTER_NOTE_MS * 2)   // 2분음표
#define EIGHTH_NOTE_MS  (QUARTER_NOTE_MS / 2)   // 8분음표
#define SIXTEENTH_NOTE_MS (QUARTER_NOTE_MS / 4) // 16분음표
#define DOTTED_QUARTER_NOTE_MS (QUARTER_NOTE_MS * 1.5) // 점4분음표

// 음표 연주 함수
void playNote(int noteFrequency, int durationMs) {
    if (noteFrequency == REST) {
        softToneWrite(BUZZER_PIN, 0); // 소리 끔
    } else {
        softToneWrite(BUZZER_PIN, noteFrequency); // 음 출력
    }
    delay(durationMs); // 해당 음표 길이만큼 지연
    softToneWrite(BUZZER_PIN, 0); // 다음 음으로 넘어가기 전에 소리 끔
    delay(10); // 음과 음 사이의 짧은 쉼 (선택 사항, 소리 분리감 향상)
}

// 악보 데이터 구조체
typedef struct {
    int note;
    int duration;
} Note;

int main() {
    // wiringPi 초기화
    if (wiringPiSetup() == -1) {
        printf("wiringPi setup failed!\n");
        return 1;
    }

    // softTone 핀 초기화
    if (softToneCreate(BUZZER_PIN) == -1) {
        printf("softToneCreate failed!\n");
        return 1;
    }

    printf("Playing Hiaishi Jo's 'Summer' melody...\n");

    // --- 'Summer' 멜로디 데이터 ---
    // 실제 악보와는 차이가 있을 수 있으며, 단순화된 멜로디입니다.
    Note summer_melody[] = {
        // 첫 번째 섹션
        {A4, SIX}, {D4, QUARTER_NOTE_MS}, {E4, QUARTER_NOTE_MS}, {G4, QUARTER_NOTE_MS},
        {E4, QUARTER_NOTE_MS}, {REST, EIGHTH_NOTE_MS}, {D4, EIGHTH_NOTE_MS}, {C4, HALF_NOTE_MS},

        {G4, QUARTER_NOTE_MS}, {E4, QUARTER_NOTE_MS}, {D4, QUARTER_NOTE_MS}, {E4, QUARTER_NOTE_MS},
        {C4, QUARTER_NOTE_MS}, {REST, EIGHTH_NOTE_MS}, {B3, EIGHTH_NOTE_MS}, {A3, HALF_NOTE_MS},

        // 반복 또는 다음 섹션 (간단화)
        {E4, QUARTER_NOTE_MS}, {D4, QUARTER_NOTE_MS}, {E4, QUARTER_NOTE_MS}, {G4, QUARTER_NOTE_MS},
        {E4, QUARTER_NOTE_MS}, {REST, EIGHTH_NOTE_MS}, {D4, EIGHTH_NOTE_MS}, {C4, HALF_NOTE_MS},

        {G4, QUARTER_NOTE_MS}, {E4, QUARTER_NOTE_MS}, {D4, QUARTER_NOTE_MS}, {E4, QUARTER_NOTE_MS},
        {C4, QUARTER_NOTE_MS}, {REST, EIGHTH_NOTE_MS}, {D4, EIGHTH_NOTE_MS}, {E4, HALF_NOTE_MS},

        // 상승하는 부분 (간단화)
        {C5, QUARTER_NOTE_MS}, {B4, QUARTER_NOTE_MS}, {A4, QUARTER_NOTE_MS}, {G4, QUARTER_NOTE_MS},
        {F4, QUARTER_NOTE_MS}, {E4, QUARTER_NOTE_MS}, {D4, QUARTER_NOTE_MS}, {C4, HALF_NOTE_MS},
        {REST, QUARTER_NOTE_MS}, // 한 박자 쉼

        // 다시 처음으로 (간단화)
        {E4, QUARTER_NOTE_MS}, {D4, QUARTER_NOTE_MS}, {E4, QUARTER_NOTE_MS}, {G4, QUARTER_NOTE_MS},
        {E4, QUARTER_NOTE_MS}, {REST, EIGHTH_NOTE_MS}, {D4, EIGHTH_NOTE_MS}, {C4, HALF_NOTE_MS},

        {G4, QUARTER_NOTE_MS}, {E4, QUARTER_NOTE_MS}, {D4, QUARTER_NOTE_MS}, {E4, QUARTER_NOTE_MS},
        {C4, QUARTER_NOTE_MS}, {REST, EIGHTH_NOTE_MS}, {B3, EIGHTH_NOTE_MS}, {A3, HALF_NOTE_MS},
    };

    int numNotes = sizeof(summer_melody) / sizeof(summer_melody[0]);

    // 멜로디 연주
    for (int i = 0; i < numNotes; i++) {
        playNote(summer_melody[i].note, summer_melody[i].duration);
    }

    printf("Melody finished.\n");

    softToneWrite(BUZZER_PIN, 0); // 최종적으로 소리 끔

    return 0;
}
