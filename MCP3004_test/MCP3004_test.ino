/*
 * M5Stackの電流センサーテスト
 * MCP3004/3008で1m秒毎に100回サンプリング
 * 3秒毎にサンプリング値や電流値をシリアルに表示
 * 値が正しそうか、ノイズがのっていないかをシリアルプロッタで確認
 */
#include <M5Stack.h>
#include <SPI.h>
#include "MCP3004.h"

#define TIMER0 0

#define SAMPLE_PERIOD 1     // サンプリング間隔(ミリ秒)
#define SAMPLE_SIZE 100     // 1ms x 100 = 100ms

hw_timer_t * samplingTimer = NULL;

const int MCP3004_CS = 2;
MCP3004 mcp3004(MCP3004_CS);

const float rl = 51.0;  // 負荷抵抗

struct amp {
    short amp_ch[4];
} amps[SAMPLE_SIZE];

volatile int t0flag;

void IRAM_ATTR onTimer0() {  // タイマ割込関数
    t0flag = 1;
}

void ampRead(uint8_t ch) {  // chのチャネルをサンプリングする
    timerAlarmEnable(samplingTimer);  // タイマを動かす
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        t0flag = 0;
        while (t0flag == 0) {  // タイマでt0flagが1になるのを待つ
            delay(0);
        }
        amps[i].amp_ch[ch] = mcp3004.read(ch);  // chの電圧値を測る
    }
    timerAlarmDisable(samplingTimer);  // タイマを止める
}

void setup(){
    M5.begin();

    Serial.begin(115200);
    while (!Serial);
    SPI.begin();  // SPIを初期化
    mcp3004.begin();  // MCP3004のオブジェクトを初期化

    samplingTimer = timerBegin(TIMER0, 80, true);  // 分周比80、1μ秒のタイマを作る
    timerAttachInterrupt(samplingTimer, &onTimer0, true);  // タイマ割込みハンドラを指定
    timerAlarmWrite(samplingTimer, SAMPLE_PERIOD * 1000, true);  // タイマ周期を設定
}

void loop() {
    ampRead(0);  // チャネル0をサンプリング
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        Serial.println(amps[i].amp_ch[0]);
    }
    delay(3000);
}
