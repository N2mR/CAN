#include <M5StickC.h>

#include "CAN_config.h"
#include "ESP32CAN.h"

#include "CanData.h"


// M5StickC plusのGroveポート
#define TX_PORT GPIO_NUM_32 
#define RX_PORT GPIO_NUM_33 

// CAN設定クラス
CAN_device_t CAN_cfg;

// プロトタイプ宣言
bool getCanData(CanData& objCanData);
std::string toString_CanData();

void setup() {
	// Initialize the M5StickC object
	Serial.begin(115200);
	M5.begin();

	// 画面の向き(0, 1, 2, 3)
	M5.Lcd.setRotation(1);	
	// テキストサイズ(1-7)
	M5.Lcd.setTextSize(2);

	// CanModule設定
	CAN_cfg.speed = CAN_SPEED_500KBPS;
    CAN_cfg.tx_pin_id = TX_PORT;
    CAN_cfg.rx_pin_id = RX_PORT;
    CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
    ESP32Can.CANInit();
}

void loop() {
	// CanDataを取得し構造体に詰める
	CanData objCanData;
	getCanData(objCanData);

	// CAN通信の状況を表示
	if (!objCanData.isNull())
	{
		// // CanDataをLCDに出力する
		M5.Lcd.setCursor(10, 0, 1);
		M5.Lcd.println(objCanData.strMsgID.c_str());
		M5.Lcd.setCursor(10, 20, 1);
		M5.Lcd.println(objCanData.strDLC.c_str());
		M5.Lcd.setCursor(10, 40, 1);
		M5.Lcd.println(objCanData.strData.c_str());
	}
}

/**
 * @details CANのデータを取得
 * 
*/
bool getCanData(CanData& objCanData){
	CAN_frame_t rx_frame;
	CAN_frame_t tx_frame;

	if(xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3*portTICK_PERIOD_MS) == pdTRUE) // <- ここでコアダンプする
	{
		// CanDataを作成する
		
		// 受信時間
		objCanData.strTime = std::to_string(millis());
		// Msg ID
		objCanData.strMsgID = std::to_string(rx_frame.MsgID);
		// DLC
		objCanData.strDLC = std::to_string(rx_frame.FIR.B.DLC);
		// CAN data
		std::string strData;
		for(int i = 0; i < 8; i++)
		{
			strData.push_back(rx_frame.data.u8[i]);
		}
		objCanData.strData = strData;
	}
	return true;
}

/**
 * @details CanDataのtoString
*/
std::string toString_CanData(CanData objCanData)
{	
	// 出力内容は下記フォーマット
	// time: xxx, MsgID: xxx, DLC: xxx, Data: xxx
	std::string strCanOutput = "time: " + objCanData.strTime + " ," + "MsgID: " + objCanData.strMsgID + " ," 
								+ "DLC: " + objCanData.strDLC + " ," + "Data: " + objCanData.strData;
	return strCanOutput;
}


