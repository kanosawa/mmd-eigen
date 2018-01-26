#ifndef MMD_MULTI_CHARA_CODE_FILE_READER_H
#define MMD_MULTI_CHARA_CODE_FILE_READER_H

#include <string>
#include <iconv.h>

using namespace std;

namespace mmd
{
	/*! @brief マルチ文字コードファイル読み込みクラス
	*/
	class MultiCharaCodeFileReader
	{
	public:
		/*! @brief コンストラクタ
		*/
		MultiCharaCodeFileReader(const string code_in);

		/*! @brief デストラクタ
		*/
		~MultiCharaCodeFileReader();

		/*! @brief ファイル入力
		* @param[in] fileStream ファイルストリーム
		* @param[in] size 入力バイト数
		* @param[in] needTrans 文字コード変換が必要か
		* @param[in] needResize リサイズが必要か
		* @return ファイルから入力した文字列
		*/
		const string fread(std::ifstream& fileStream, const int size, const bool needTrans, const bool needResize) const;

	private:
		/*! @brief デフォルトコンストラクタの禁止
		*/
		MultiCharaCodeFileReader();

		/*! @brief コピーコンストラクタの禁止
		*/
		MultiCharaCodeFileReader(const MultiCharaCodeFileReader&);

		/*! @brief 代入演算子の禁止
		*/
		MultiCharaCodeFileReader& operator=(const MultiCharaCodeFileReader&);

		/*! @brief 変換記述子
		*/
		iconv_t conv_;
	};

}

#endif