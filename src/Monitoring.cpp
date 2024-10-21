#include "Monitoring.h"


Monitoring::Monitoring(CAN_device_t& CAN_cfg)
    :m_pParser(), m_CAN_cfg(CAN_cfg)
{
    makeInstance();
}

std::string Monitoring::monitoringCanData()
{
    std::string strRet = "";
    // CANフレームが存在していればデータを取得する
	if (existsCanFrame())
	{
		// CanDataを文字列取得
		strRet = getCanData();
	}

    return strRet;
}

bool Monitoring::makeInstance()
{
    std::vector<CANID> lstMonitoringTarget = { CANID::RPM };
    m_pParser = std::make_shared<Parser>(lstMonitoringTarget);
    return true;
}

/**
 * @details CanFrameの存在確認
*/
bool Monitoring::existsCanFrame()
{
	CAN_frame_t rx_frame;
	std::string strMsgID;
	// メッセージIDが存在する場合はフレームが存在すると判定
	if(xQueueReceive(m_CAN_cfg.rx_queue, &rx_frame, 3*portTICK_PERIOD_MS) == pdTRUE)
	{
		strMsgID = std::to_string(rx_frame.MsgID);
	}

	return !strMsgID.empty()? true : false;
}

std::string Monitoring::getCanData()
{
	std::string strOutputCanData = "";
	// CanFrame
	CAN_frame_t rx_frame;

	if(xQueueReceive(m_CAN_cfg.rx_queue, &rx_frame, 3*portTICK_PERIOD_MS) == pdTRUE)
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
		uint64_t uData = 0x00;
		for(uint8_t i = 0 ; i < DLC; i++)
		{
			uint8_t data = static_cast<int>(rx_frame.data.u8[i]);
			std::bitset<8> binaryData(data);
			std::string strBinaryData = binaryData.to_string();
			strData.append(std::string(strBinaryData));
			strData.append(",");

			// 一律でuint64_tに変換
			uData = uData << 8 | rx_frame.data.u8[i];
		}

		// 出力文字列の形成
		if (rx_frame.MsgID == 256)
		{
			uint32_t rpm = (uData >> 40) & 0xFFFFFF;

			uint32_t throttle = (uData >> 32) & 0x000000FF;

			strOutputCanData = "{\"rpm\":\"" + std::to_string(rpm) + "\",\"throttle\":\"" + std::to_string(throttle) + "\"}";

			return strOutputCanData;
		}
		else
		{
			// 目的のMsgIDと異なる場合、空文字列を返す
			return "";
		}
	}
	return strOutputCanData;
}