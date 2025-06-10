#include <stdio.h>
#include <wiringPi.h>
#include <softTone.h>

// 부저가 연결된 GPIO 핀 번호 (wiringPi GPIO 번호 사용)
#define BUZZER_PIN 6 // 예시: wiringPi GPIO 1번 (BCM 18)
#define SWITCH_PIN 5 // 스위치 연결 핀

// --- 음계 주파수 정의 (C4 = 미들 C) ---
// 옥타브 3 (낮은 음)
#define C3 131
#define C3_SHARP 139
#define D3 147
#define D3_SHARP 156
#define E3 165
#define F3 175
#define F3_SHARP 185
#define G3 196
#define G3_SHARP 208
#define A3 220
#define A3_SHARP 233
#define B3 247

// 옥타브 4 (중간 음)
#define C4 262
#define C4_SHARP 277
#define D4 294
#define D4_SHARP 311
#define E4 330
#define F4 349
#define F4_SHARP 370
#define G4 392
#define G4_SHARP 415
#define A4 440
#define A4_SHARP 466
#define B4 494
#define B4_SHARP 523

// 옥타브 5 (높은 음)
#define C5 523
#define C5_SHARP 554
#define D5 587
#define D5_SHARP 622
#define E5 659
#define F5 698
#define F5_SHARP 740
#define G5 784
#define G5_SHARP 831
#define A5 880
#define A5_SHARP 932
#define B5 988
#define B5_SHARP 1047

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

    // 스위치 핀 설정
    pinMode(SWITCH_PIN, INPUT);
    pullUpDnControl(SWITCH_PIN, PUD_UP); // 풀업 저항 활성화

    printf("Playing Hiaishi Jo's 'Summer' melody...\n");
    printf("Press the switch to stop the melody.\n");

    // --- 'Summer' 멜로디 데이터 ---
    // 실제 악보와는 차이가 있을 수 있으며, 단순화된 멜로디입니다.
    Note summer_melody[] = {
        // 첫 번째 마디
        {A4, SIXTEENTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS},
        {E5, SIXTEENTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS},

        // 두 번째 마디
        {E5, EIGHTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS+QUARTER_NOTE_MS},
        {REST, QUARTER_NOTE_MS},
        
        {A4, SIXTEENTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS},
        {E5, SIXTEENTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS},

        // 세 번째 마디
        {E5, EIGHTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS},
        {E5, EIGHTH_NOTE_MS},
        {F5_SHARP, EIGHTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS+QUARTER_NOTE_MS},
        {A4, SIXTEENTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS},
        {E5, SIXTEENTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS},

        //네 번째 마디
        {E5, EIGHTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS+QUARTER_NOTE_MS},
        {REST, QUARTER_NOTE_MS},
        
        {A4, SIXTEENTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS},
        {E5, SIXTEENTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS},

        // 다섯 번째 마디
        {E5, EIGHTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS},
        {E5, EIGHTH_NOTE_MS},
        {A5, EIGHTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS+QUARTER_NOTE_MS},
        {F5_SHARP, EIGHTH_NOTE_MS},
        {G5, EIGHTH_NOTE_MS},

        // 여섯 번째 마디
        {A5, EIGHTH_NOTE_MS},
        {A5, SIXTEENTH_NOTE_MS},
        {A5, SIXTEENTH_NOTE_MS+EIGHTH_NOTE_MS},
        {A5, EIGHTH_NOTE_MS},
        {A5, EIGHTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS+EIGHTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS},
        {G5, SIXTEENTH_NOTE_MS},
        
        // 일곱 번째 마디
        {A5, EIGHTH_NOTE_MS},
        {A5, SIXTEENTH_NOTE_MS},
        {A5, SIXTEENTH_NOTE_MS+EIGHTH_NOTE_MS},
        {A5, EIGHTH_NOTE_MS},
        {A5, EIGHTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS+EIGHTH_NOTE_MS},
        {D5, SIXTEENTH_NOTE_MS},
        {E5, SIXTEENTH_NOTE_MS},

        // 여덟 번째 마디
        {F5_SHARP, EIGHTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS},
        {F5_SHARP, SIXTEENTH_NOTE_MS+EIGHTH_NOTE_MS},
        {F5_SHARP, EIGHTH_NOTE_MS},
        {F5_SHARP, EIGHTH_NOTE_MS},
        {B5, EIGHTH_NOTE_MS},
        {B5, SIXTEENTH_NOTE_MS},
        {F5_SHARP, EIGHTH_NOTE_MS},
        {E5, SIXTEENTH_NOTE_MS},
        {D5, EIGHTH_NOTE_MS}
    };

    int numNotes = sizeof(summer_melody) / sizeof(summer_melody[0]);

    // 멜로디 연주
    for (int i = 0; i < numNotes; i++) {
        // 스위치가 눌렸는지 확인 (LOW 상태가 눌림)
        if (digitalRead(SWITCH_PIN) == LOW) {
            printf("\nSwitch pressed. Stopping melody...\n");
            break;
        }
        playNote(summer_melody[i].note, summer_melody[i].duration);
    }

    printf("Melody finished.\n");
    softToneWrite(BUZZER_PIN, 0); // 최종적으로 소리 끔

    return 0;
}
