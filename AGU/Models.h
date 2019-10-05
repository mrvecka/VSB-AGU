#pragma once
#include <opencv2/opencv.hpp>
#include "iostream";
#include "stdio.h";

using namespace cv;
using namespace std;

enum POINT_TYPE {
	LEFT = 0,
	RIGHT = 1,
	INTERSECTION =2
};

class Line
{
public:
	Line(string label, int left_x, int left_y, int right_x, int right_y, cv::Scalar color);
	Line();

	Point GetLeftPoint();
	Point GetRightPoint();
	string GetLabel();
	cv::Scalar GetColor();

private:
	string label;
	int left_x;
	int left_y;
	int right_x;
	int right_y;
	cv::Scalar color;

};


class OrderItem
{
public:
	OrderItem(Point p, Line* l, POINT_TYPE left);
	OrderItem();
	Line* GetCurrentLine();
	Point GetPoint();

	POINT_TYPE Type;
	Line* GetIntersectionLineS1();
	Line* GetIntersectionLineS2();
	void SetIntersectionLines(Line* s1, Line* s2);
	bool operator <(const OrderItem& line) const;

private:
	Point point;
	Line* line;

	Line* s1;
	Line* s2;

};


class PriorityQueue
{
public:
	PriorityQueue();
	~PriorityQueue();
	void Insert(OrderItem* l);
	void Delete(OrderItem* l);
	Line* Above(Point p, list<Line*> line_state, string label);
	Line* Below(Point p, list<Line*> line_state, int lowerEdge, string label);
	bool Member(OrderItem* item);

	OrderItem* MIN_Elem();
	bool GetElementsForPolygon(list<OrderItem*>* items);

	list<OrderItem*> GetListItems();
	void SortElements();
	void SortForPolygon();

private:
	list<OrderItem*> orderItems;
	list<OrderItem*>::iterator it;
};


