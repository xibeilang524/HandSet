/// @file
///     std52_ft3_msg.h
///
/// @brief
///     标准FT3采样值报文帧（52字节版本,含CRC）的编解码接口及接口对象实例的创建和释放接口
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.09.05
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef STD52_FT3_MESSAGE_H_

#define STD52_FT3_MESSAGE_H_

#include "std52_ft3_types.h"

///
/// @brief
///     标准FT3采样值报文（52字节版本,含CRC）的编解码接口
///
class Std52FT3Msg
{
public:
	virtual ~Std52FT3Msg(){};

	///
	/// @brief
	///     对捕获的标准FT3采样值报文（52字节版本,含CRC）帧进行解码
	///
	/// @param[in]
	///     msgDataBuf - 保存捕获的报文帧数据的缓存
	/// @param[in]
	///     bufSize    - 缓存大小
	///
	/// @return
	///     实际的解码数据大小 - 成功； 0 - 失败
	///
	virtual size_t Decode(const uint16_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     根据设置的参数进行编码，获得标准FT3采样值报文（52字节版本）帧的ASDU（即不含CRC及启动符）
	///
	/// @param[in]
	///     msgDataArr - 保存报文帧数据的数组
	/// @param[in]
	///     arrSize    - 数组大小
	///
	/// @return
	///     保存报文帧数据的数组的实际大小 - 成功； 0 - 失败
	///
	virtual size_t Code(uint16_t *msgDataArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     获取ASDU的长度
	///
	/// @return
	///     ASDU的长度
	///
	virtual size_t GetASDULen() = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU的公共信息，即ASDU中的采样值数据以外的部分
	///
	/// @param[in]
	///     asduPublic - 保存ASDU的公共信息的结构体
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool GetASDUPublicInfo(STD52_FT3_ASDU_PUBLIC &asduPublic) = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU中的采样值数据部分的通道数
	///
	/// @return
	///     ASDU中的采样值数据部分的通道数
	///
	virtual size_t GetASDUSmpsNum() = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU中的采样值数据
	///
	/// @param[in]
	///     smpValArr  - 保存采样瞬时值的数组
	/// @param[in]
	///     arrSize    - 数组大小,其值需>=通道数
	///
	/// @return
	///     数组的实际大小
	///
	virtual size_t GetASDUSmps(uint16_t *smpValArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     获取ASDU在报文帧中的偏移地址
	///
	/// @return
	///     ASDU在报文帧中的偏移地址
	///
	virtual size_t GetASDUOff() = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU的公共信息在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     asduPublic - 保存ASDU的公共信息的偏移地址信息的结构体
	///
	/// @return
	///     
	///
	virtual void GetASDUPublicInfoOff(STD52_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff) = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU中的采样值数据在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     smpOffArr - 保存采样瞬时值的偏移地址信息的数组
	/// @param[in]
	///     arrSize   - 数组的大小,其值需>=通道数
	///
	/// @return
	///     数组的实际大小
	///
	virtual size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     设置报文帧中的ASDU信息
	///
	/// @param[in]
	///     asduPublic - ASDU的公共信息
	/// @param[in]
	///     smpValBuf  - 保存采样瞬时值的缓存
	/// @param[in]
	///     bufSize    - 缓存大小
	///
	/// @return
	///     
	///
	virtual void SetASDU(const STD52_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t bufSize) = 0;
};

///
/// @brief
///     创建标准FT3采样值报文（52字节版本,含CRC）的编解码接口对象的实例
///
/// @return
///     指向Std52FT3Msg实例的指针
///
Std52FT3Msg *CreateStd52FT3MsgImpl();

///
/// @brief
///     释放标准FT3采样值报文（52字节版本,含CRC）的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向Std52FT3Msg实例的指针
///
/// @return
///     
///
void ReleaseStd52FT3MsgImpl(Std52FT3Msg *impl);

#endif
