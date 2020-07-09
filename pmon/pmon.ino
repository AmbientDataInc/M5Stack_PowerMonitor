/*
 * M5Stackの電流センサー
 * MCP3004/3008で1m秒毎に100回サンプリング
 * 60秒毎に電流値をLCDに表示する
 */
#include <M5Stack.h>
#include <SPI.h>
#include "MCP3004.h"
#include <math.h>

#define TIMER0 0

#define PERIOD 60           // 送信間隔(秒)
#define SAMPLE_PERIOD 1     // サンプリング間隔(ミリ秒)
#define SAMPLE_SIZE 100     // 1ms x 100 = 100ms

hw_timer_t * samplingTimer = NULL;

const int MCP3004_CS = 2;
MCP3004 mcp3004(MCP3004_CS);

const float rl = 51.0; // Load Resistance

volatile int t0flag;

void IRAM_ATTR onTimer0() {
    t0flag = 1;
}

// chのチャネルをサンプリングする

float ampRead(uint8_t ch) {
    int vt;
    float amp, ampsum;
    ampsum = 0;

    timerAlarmEnable(samplingTimer);  // タイマを動かす
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        t0flag = 0;
        while (t0flag == 0) {  // タイマでt0flagが1になるのを待つ
            delay(0);
        }
        vt = mcp3004.read(ch);  // chの電圧値を測る
        amp = (float)(vt - 512) / 1024.0 * 3.3 / rl * 2000.0;  // 電流値を計算
        ampsum += amp * amp;  // 電流値を2乗して足し込む
    }
    timerAlarmDisable(samplingTimer);  // タイマを止める

    return ((float)sqrt((double)(ampsum / SAMPLE_SIZE)));  // 電流値の2乗を平均し、平方根を計算
}

// リングバッファ
#define NDATA 100  // リングバッファの件数
struct d {
    bool valid;
    float d1;
    float d2;
} data[NDATA];  // リングバッファ
int dataIndex = 0;  // リングバッファのインデクス

void putData(float d1, float d2) {  // リングバッファにデータを挿入する
    if (++dataIndex >= NDATA) {
        dataIndex = 0;
    }
    data[dataIndex].valid = true;
    data[dataIndex].d1 = d1;
    data[dataIndex].d2 = d2;
}

#define X0 10

int data2y(float d, float minY, float maxY, int HEIGHT) {  // データの値からy軸の値を計算する
    return HEIGHT - ((int)((d - minY) / (maxY - minY) * (float)HEIGHT) + 1);
}

void drawChart() {  // リングバッファからデータを読み、グラフ表示する
    int HEIGHT = M5.Lcd.height() - 10;
    float mind = 0.0, maxd = 10.0;
    for (int i = 0; i < NDATA; i++) {  // リングバッファ中の最大値を探す
        if (data[i].valid == false) continue;
        if (data[i].d1 > maxd) maxd = data[i].d1;
        if (data[i].d2 > maxd) maxd = data[i].d2;
    }
    maxd *= 1.1;
    for (int i = 0, j = dataIndex + 1; i < (NDATA - 1); i++, j++) {
        if (data[j % NDATA].valid == false) continue;
        int d10 = data2y(data[j % NDATA].d1, mind, maxd, HEIGHT);
        int d11 = data2y(data[(j + 1) % NDATA].d1, mind, maxd, HEIGHT);
        M5.Lcd.drawLine(i * 3 + X0, d10, (i + 1) * 3 + X0, d11, BLUE);  // 系統1のグラフを描く
        int d20 = data2y(data[j % NDATA].d2, mind, maxd, HEIGHT);
        int d21 = data2y(data[(j + 1) % NDATA].d2, mind, maxd, HEIGHT);
        M5.Lcd.drawLine(i * 3 + 1 + X0, d20, (i + 1) * 3 + 1 + X0, d21, RED);  // 系統2のグラフを描く
    }
}

void setup(){
    M5.begin();

    Serial.begin(115200);
    while (!Serial);
    SPI.begin();

    mcp3004.begin();

    samplingTimer = timerBegin(TIMER0, 80, true);
    timerAttachInterrupt(samplingTimer, &onTimer0, true);
    timerAlarmWrite(samplingTimer, SAMPLE_PERIOD * 1000, true);

    for (int i; i < NDATA; i++) {
        data[i].valid = false;
    }
}

void loop() {
    unsigned long t = millis();  // 開始時刻を記録する
    float a0, a1;
    a0 = ampRead(0);  // 系統1の電流値を測定する
    a1 = ampRead(1);  // 系統2の電流値を測定する

    putData(a0, a1);  // 電流値をリングバッファに挿入する

    M5.Lcd.fillScreen(BLACK);  // LCDをクリアする
    drawChart();  // LCDにグラフを表示する

    Serial.print(a0);
    Serial.print(" ");
    Serial.println(a1);
    while ((millis() - t) < PERIOD * 1000) {
        delay(0);
    }
}
