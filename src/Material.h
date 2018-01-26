#ifndef MMD_MATERIAL_H
#define MMD_MATERIAL_H

#include <Eigen/Dense>

namespace mmd
{
	/*! @brief マテリアルクラス
	*/
	class Material
	{
	public:
		/*! @brief コンストラクタ
		*/
		Material();

		/*! @brief コピーコンストラクタ
		*/
		Material(const Material&);

		/*! @brief デストラクタ
		*/
		~Material();

		/*! @brief 代入演算子
		*/
		Material& operator=(const Material&);

		/*! @brief ディフューズの設定
		* @param[in] diffuse ディフューズ
		*/
		void setDiffuse(const Eigen::Vector4f& diffuse);

		/*! @brief ディフューズの取得
		* @return ディフューズ
		*/
		const Eigen::Vector4f& getDiffuse() const;

		/*! @brief スペキュラーの設定
		* @param[in] specular スペキュラー
		*/
		void setSpecular(const Eigen::Vector3f& specular);

		/*! @brief スペキュラーの取得
		* @return スペキュラー
		*/
		const Eigen::Vector3f& getSpecular() const;

		/*! @brief スペキュラー係数の設定
		* @param[in] specularCoef スペキュラー係数
		*/
		void setSpecularCoef(const float specularCoef);

		/*! @brief スペキュラー係数の取得
		* @return スペキュラー係数
		*/
		const float getSpecularCoef() const;

		/*! @brief アンビエントの設定
		* @param[in] ambient アンビエント
		*/
		void setAmbient(const Eigen::Vector3f& ambient);

		/*! @brief アンビエントの取得
		* @return アンビエント
		*/
		const Eigen::Vector3f& getAmbient() const;

		/*! @brief 通常テクスチャインデックスの設定
		* @param[in] ordinaryTextureIndex 通常テクスチャインデックス
		*/
		void setOrdinaryTextureIndex(const unsigned char ordinaryTextureIndex);

		/*! @brief 通常テクスチャインデックスの取得
		* @return 通常テクスチャインデックス
		*/
		const unsigned char getOrdinaryTextureIndex() const;

		/*! @brief 面数の設定
		* @param[in] surfaceNum 面数
		*/
		void setSurfaceNum(const int surfaceNum);

		/*! @brief 面数の取得
		* @return 面数
		*/
		const int getSurfaceNum() const;

	private:
		/*! @brief コピー関数
		* param[in] material コピー元マテリアル
		*/
		void copy(const Material& material);

		/*! @brief ディフューズ
		*/
		Eigen::Vector4f diffuse_;

		/*! @brief スペキュラー
		*/
		Eigen::Vector3f specular_;

		/*! @brief スペキュラー係数
		*/
		float specularCoef_;

		/*! @brief アンビエント
		*/
		Eigen::Vector3f ambient_;

		/*! @brief 通常テクスチャインデックス
		*/
		unsigned char ordinaryTextureIndex_;

		/*! @brief 面数
		*/
		int surfaceNum_;
	};
}

#endif