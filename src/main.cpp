#include <M5StickC.h>
#include <bitset>

#include "CAN_config.h"
#include "ESP32CAN.h"

#include "BluetoothSerial.h"


// M5StickC plusのGroveポート
#define TX_PORT GPIO_NUM_32 
#define RX_PORT GPIO_NUM_33

// CAN設定クラス
CAN_device_t CAN_cfg;
// Bluetooth
BluetoothSerial SerialBT;

// プロトタイプ宣言
bool existsCanFrame();
std::string getCanData();

void setup() {
	// Initialize the M5StickC objects
	Serial.begin(9600);
	SerialBT.begin("M5StickC");
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
	// 生存確認
	M5.Lcd.setCursor(10, 0, 1);
	std::string running = "running";
	M5.Lcd.println(running.c_str());

	// CAN通信の状況を表示
	if (existsCanFrame())
	{
		// // CanDataをLCDに出力する
		// M5.Lcd.setCursor(10, 0, 1);
		// M5.Lcd.println(objCanData.strMsgID.c_str());
		// M5.Lcd.setCursor(10, 20, 1);
		// M5.Lcd.println(objCanData.strDLC.c_str());
		// M5.Lcd.setCursor(10, 40, 1);
		// M5.Lcd.println(objCanData.strData.c_str());

		// CanDataを文字列取得
		std::string strOutputCanData = getCanData();
		// Bluetoothで文字列を送信
		SerialBT.println(strOutputCanData.c_str());
	}
	else
	{
		SerialBT.println("No Signal.");
	}
}

/**
 * @details CanFrameの存在確認
*/
bool existsCanFrame()
{
	CAN_frame_t rx_frame;
	std::string strMsgID;
	// メッセージIDが存在する場合はフレームが存在すると判定
	if(xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3*portTICK_PERIOD_MS) == pdTRUE)
	{
		strMsgID = std::to_string(rx_frame.MsgID);
	}

	return !strMsgID.empty()? true : false;
}

std::string getCanData()
{
	// CanFrame
	CAN_frame_t rx_frame;

	if(xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3*portTICK_PERIOD_MS) == pdTRUE)
	{
		// 受信時間
		std::string strTime = std::to_string(millis());
		// Msg ID
		std::string strMsgID = std::to_string(rx_frame.MsgID);
		// DLC
		uint8_t DLC = rx_frame.FIR.B.DLC;
		std::string strDLC = std::to_string(DLC);
		// CAN data u8
		std::string strData;
		for(uint8_t i = 0 ; i < DLC; i++)
		{
			uint8_t data = static_cast<int>(rx_frame.data.u8[i]);
			// constexpr int bufferSize = sizeof(int) * 2 + 1;  // 整数のバイト数 × 2 + 1（null終端文字列の分）
			// char buffer[bufferSize];
			
			// // sprintfを使用して整数を16進数文字列に変換
			// sprintf(buffer, "%X", data);

			std::bitset<8> binaryData(data);
			std::string strBinaryData = binaryData.to_string();

			strData.append(std::string(strBinaryData));
			strData.append(",");
		}

		// 出力文字列の形成
		std::string strOutputCanData = "time: " + strTime + "  " + "MsgID: " + strMsgID + "  " + "data: " + strData;

		return strOutputCanData;
	}
}