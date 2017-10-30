#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <memory>
#include <climits>
#include <stdexcept>

namespace Apricot
{
	class ColorWheel
	{
	public:
		/** Constructor */
		explicit ColorWheel (float huePrecision)
		{
			if (huePrecision * UINT_MAX < 360)
				throw std::logic_error ("精度过高 | 色相值将不正确");

			// if ((cHue / 6) * 6 != cHue)
			// 	throw std::exception ("不能整除 | 色相尾值将不正确");

			mHuePrecision = huePrecision;
			cHue = static_cast<unsigned int> (360 / huePrecision);

			//!< 色相值
			pHueIndex = new float*[cHue];
			unsigned int c = 0;
			try
			{
				for (c = 0; c < cHue; ++c)
					pHueIndex[c] = new float[3];
			}
			catch (std::bad_alloc &e)
			{
				for (unsigned int x = 0; x < c; ++x)
					delete[] pHueIndex[x];
				delete[] pHueIndex;
				throw;
			}

			//!< 颜色轮起点 R->Y->G->C->B->M->R
			//!< 用起点表示该段颜色
			unsigned int const divcol = cHue / 6;
			for (c = 0; c < divcol; ++c)
			{
				float cdf = static_cast<float> (c) / divcol;
				float* R = pHueIndex[0 * divcol + c];
				float* Y = pHueIndex[1 * divcol + c];
				float* G = pHueIndex[2 * divcol + c];
				float* C = pHueIndex[3 * divcol + c];
				float* B = pHueIndex[4 * divcol + c];
				float* M = pHueIndex[5 * divcol + c];
				// R							G								B
				R[0] = 1.0f;				R[1] = cdf; 				R[2] = 0.0f;
				Y[0] = 1.0f - cdf;		Y[1] = 1.0f;				Y[2] = 0.0f;
				G[0] = 0.0f; 			G[1] = 1.0f;			G[2] = cdf;
				C[0] = 0.0f; 			C[1] = 1.0f - cdf;	C[2] = 1.0f;
				B[0] = cdf;				B[1] = 0.0f;				B[2] = 1.0f;
				M[0] = 1.0f;			M[1] = 0.0f; 			M[2] = 1.0f - cdf;
			}

			//!< 处理色相轮尾部 | 设为红色
			for (c = 6 * divcol; c < cHue; ++c)
			{
				float* M = pHueIndex[c];
				M[0] = 1.0f; M[1] = 0.0f; M[2] = 0.0f;
			}
		}

		/** Default destructor */
		virtual ~ColorWheel()
		{
			unsigned int c = 0;
			for (c = 0; c < cHue; ++c)
				delete[] pHueIndex[c];
			delete[] pHueIndex;
		}

		/** \brief 获取色相精度
		 * \return mHuePrecision
		 */
		float getmHuePrecision() const { return mHuePrecision; }

		/** \brief 获取色相数量
		 * \return cHue
		 */
		unsigned int getcHue() const { return cHue; }

		/** \brief 取得色相轮中颜色
		 *
		 * \param [in] h 色相索引
		 * \param [in] c 红蓝绿 (0/1/2) 通道
		 * \return 所需要的值 (0.0f->1.0f)
		 */
		float at (unsigned int c, unsigned int x)
		{
			if (c > cHue)
				throw std::out_of_range ("c: 色相索引越界");
			else if (x > 2)
				throw std::out_of_range ("x: 通道索引越界");
			else
				return pHueIndex[c][x];
		}

		/** \brief 转换 HSV 到 RGB (0.0f->1.0f) 根据 colorwheel 映射
		 *
		 * \param [in] h
		 * \param [in] s
		 * \param [in] v
		 * \param [out] r
		 * \param [out] g
		 * \param [out] b
		 * \return transform successfully or not
		 */
		bool HSV2RGBMap (float const& h, float const& s, float const& v, float& r, float& g, float& b)
		{
			if (h < 0.0f || h > 360.0f || s < 0.0f || s > 1.0f || v < 0.0f || v > 1.0f)
				return false;
			unsigned int hDown = static_cast<int> (h / mHuePrecision);
			float* pCol = pHueIndex[hDown];
			// 引入饱和度和色调 | 缩放 RGB 范围
			// y-v(1-s) = [v-v(1-s)]/(1-0)x = v-vs+vs *x
			float vs = v * s; float vvs = v - vs;
			r = vvs + vs * pCol[0];
			g = vvs + vs * pCol[1];
			b = vvs + vs * pCol[2];
			return true;
		}

		/** \brief 转换 HSV 到 RGB (0.0f->1.0f) 通过实际插值计算
		 *
		 * \param [in] h
		 * \param [in] s
		 * \param [in] v
		 * \param [out] r
		 * \param [out] g
		 * \param [out] b
		 * \return transform successfully or not
		 */
		static bool HSV2RGBCalc (float const& h, float const& s, float const& v, float& r, float& g, float& b)
		{
			if (h < 0.0f || h > 360.0f || s < 0.0f || s > 1.0f || v < 0.0f || v > 1.0f)
				return false;
			static int const pHueCalc[6][3] =
			{
				1, 0, 0, // R
				1, 1, 0, // Y
				0, 1, 0, // G
				0, 1, 1, // C
				0, 0, 1, // B
				1, 0, 1 // M
			}; //!< 用于计算的参考点
			float hf = h / 60.0f;
			unsigned int hDown = static_cast<unsigned int> (hf);
			unsigned int hUp = (hDown + 1) % 6;
			hf = hf - hDown;
			int const* pColDown = pHueCalc[hDown];
			int const* pColUp = pHueCalc[hUp];
			// 线性插值
			r = (1.0 - hf) * pColDown[0] + hf * pColUp[0];
			g = (1.0 - hf) * pColDown[1] + hf * pColUp[1];
			b = (1.0 - hf) * pColDown[2] + hf * pColUp[2];
			// 引入饱和度和色调 | 缩放 RGB 范围
			// y-v(1-s) = [v-v(1-s)]/(1-0)x = v-vs+vs *x
			float vs = v * s; float vvs = v - vs;
			r = vvs + vs * r;
			g = vvs + vs * g;
			b = vvs + vs * b;
			return true;
		}

	protected:

	private:
		float mHuePrecision; //!< 色相精度 "mHuePrecision"
		unsigned int cHue; //!< 色相数量 "cHue"
		ColorWheel& operator= (ColorWheel) {}; //!< 禁止赋值运算符
		ColorWheel (ColorWheel&) {}; //!< 禁止复制构造函数
		float** pHueIndex; //!< 色相数据 "pHueIndex" | 考虑 public 以省去 at() 函数
	};
}

#endif // COLORWHEEL_H
