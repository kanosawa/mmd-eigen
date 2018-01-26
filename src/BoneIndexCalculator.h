#ifndef MMD_BONE_INDEX_CALCULATOR_H
#define MMD_BONE_INDEX_CALCULATOR_H

#include <map>
#include "Bone.h"

namespace mmd
{
	/*! @brief ボーンインデックス算出クラス
	*/
	class BoneIndexCalculator
	{
	public:
		/*! @brief コンストラクタ
		* @param[in] bones ボーン群
		*/
		BoneIndexCalculator(const vector<Bone>& bones);

		/*! @brief デストラクタ
		*/
		~BoneIndexCalculator();

		/*! @brief ボーンインデックスの算出
		* @param[out] index ボーンインデックス
		* @param[in] boneName ボーン名
		* @return 成否
		*/
		bool process(int& index, const string& boneName);

	private:
		/*! @brief デフォルトコンストラクタの禁止
		*/
		BoneIndexCalculator();

		/*! @brief コピーコンストラクタの禁止
		*/
		BoneIndexCalculator(const BoneIndexCalculator&);

		/*! @brief 代入演算子の禁止
		*/
		BoneIndexCalculator& operator=(const BoneIndexCalculator&);

		/*! @brief 連想配列
		*/
		map<string, int> name2Index_;
	};

}

#endif