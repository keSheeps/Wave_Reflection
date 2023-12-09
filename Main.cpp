# include <Siv3D.hpp>

//右向きの波と左向きの波の配列を用意して計算 表示は加算
//distanceだけdataをシフトした配列を返す(波の移動)
Array<double> shiftValues(Array<double> data, int distance) {
	Array<double> result(data.size());
	for (int i = 0; i < data.size(); i++) {
		if (0 <= i + distance and i + distance < data.size()) {
			result[i + distance] = data[i];
		}
	}
	return result;
}
//distanceだけdataをシフトして折り返した配列を返す(波の反射)
Array<double> reflectValues(Array<double> data, int distance, bool reverse) {
	Array<double> result(data.size());
	for (int i = 0; i < data.size(); i++) {
		if (data.size() <= i + distance and distance>0) {
			result[data.size() - 1 - (i + distance - data.size())] = data[i] * (reverse?-1:1);
		}
		if (i + distance < 0 and distance<0) {
			result[-1-(i+distance)] = data[i] * (reverse ? -1 : 1);
		}
	}
	return result;
}

Array<double> addArray(Array<double> array1, Array<double> array2) {
	Array<double> result(array1.size());
	for (int i = 0; i < array1.size(); i++) {
		result[i] = array1[i] + array2[i];
	}
	return result;
}

void DrawLineGraph(const Rect& graphArea, const Array<double>& values, double maxValue, double minValue, const ColorF& color, double thickness)
{
	const double xStep = (graphArea.w / (values.size() - 1.0));
	const double yStep = (graphArea.h/2 / maxValue - minValue);

	LineString lines;

	for (size_t i = 0; i < values.size(); ++i)
	{
		const double x = (graphArea.x + xStep * i);
		const double y = (graphArea.y + graphArea.h/2 - yStep * values[i]);

		lines << Vec2{ x, y };
	}

	lines.draw(LineStyle::RoundCap, thickness, color);
}

void Main()
{Scene::SetBackground(Palette::White);

	const Rect graphArea{ 40, 40, 600, 400 };

	const int arraySize = 600;
	Array<double> toRight(arraySize);
	Array<double> toLeft(arraySize);
	Array<double> tempToRight(arraySize);
	Array<double> tempToLeft(arraySize);
	//波生成
	for (int i = 0; i < arraySize/2; i++) {
		//toRight[i] = -Math::Sin(Math::Pi * 2 / (arraySize/2) * i);
	}

	double T = 1;//秒
	double lambda = 300;//配列の数
	double beforeTime = Scene::Time();
	while (System::Update())
	{
		//枠を描画
		//graphArea.left().draw(ColorF{ 0.11 })
		graphArea.bottom().draw(ColorF{0.11});
		graphArea.top().draw(ColorF{0.11});
		graphArea.right().draw(ColorF{ 0.11 });

		const double deltaT = Scene::Time() - beforeTime;
		if (deltaT > T/lambda) {
			//グラフを移動させる
			const int step = lambda / T * deltaT;
			for (int i = 0; i < step; i++) {
				tempToRight = shiftValues(toRight, 1);
				tempToLeft = shiftValues(toLeft, -1);

				tempToRight = addArray(tempToRight, reflectValues(toLeft, -1, false));
				toLeft = addArray(tempToLeft, reflectValues(toRight, 1, true));
				toRight = tempToRight;

				toRight[0] += Math::Sin(Math::Pi * 2 / T * (beforeTime + (deltaT / step * i)));//波を足す(外からの音源)
			}
			beforeTime = Scene::Time();

		}

		//和を描画
		const Array<double> sum = addArray(toRight, toLeft);
		//max,min
		DrawLineGraph(graphArea, sum, 8, -8, HSV{ 160, 1.0, 0.9 }, 4);
	}
}
