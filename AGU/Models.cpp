#include "pch.h"
#include "Models.h"

Line::Line(string label, int left_x, int left_y, int right_x, int right_y)
{
	this->label = label;
	this->left_x = left_x;
	this->left_y = left_y;
	this->right_x = right_x;
	this->right_y = right_y;
}

Line::Line()
{}

Point Line::GetLeftPoint()
{
	return Point(this->left_x, this->left_y);
}

Point Line::GetRightPoint()
{
	return Point(this->right_x, this->right_y);
}

string Line::GetLabel()
{
	return this->label;
}

OrderItem::OrderItem(Point p, Line* l, POINT_TYPE type)
{
	this->point = p;
	this->line = l;
	this->Type = type;
}


Line* OrderItem::GetCurrentLine()
{
	return this->line;
}

Point OrderItem::GetPoint()
{
	return this->point;
}

bool OrderItem::operator<(const OrderItem& item) const
{
	return this->point.x < item.point.x;
}

Line* OrderItem::GetIntersectionLineS1()
{
	return this->s1;
}

Line* OrderItem::GetIntersectionLineS2()
{
	return this->s2;
}

void OrderItem::SetIntersectionLines(Line* s1, Line* s2)
{
	this->s1 = s1;
	this->s2 = s2;
}

PriorityQueue::~PriorityQueue()
{
	for (this->it = this->orderItems.begin(); it != this->orderItems.end(); it++)
	{
		delete &this->it;
	}
}
PriorityQueue::PriorityQueue(){}

void PriorityQueue::Insert(OrderItem* i)
{
	this->orderItems.push_back(i);
}

void PriorityQueue::Delete(OrderItem* i)
{
	for (this->it = this->orderItems.begin(); it != this->orderItems.end(); it++)
	{
		if ((*this->it)->GetCurrentLine()->GetLabel() == i->GetCurrentLine()->GetLabel())
		{
			delete (*this->it)->GetCurrentLine();
			delete (*this->it)->GetIntersectionLineS1();
			delete (*this->it)->GetIntersectionLineS2();

			delete &this->it;
		}
	}
}

Line* PriorityQueue::Above(Point p, list<Line*> line_state, string label)
{
	Line* above = NULL;
	for (int y = p.y; y >= 0; y--)
	{
		Point tested = Point(p.x,y);
		for (list<Line*>::iterator it = line_state.begin(); it != line_state.end(); it++)
		{

			Point left = (*it)->GetLeftPoint();
			Point right = (*it)->GetRightPoint();

			//float AB = sqrt((right.x - left.x)*(right.x - left.x) + (right,y - left.y)*(right.y - left.y));
			//float AP = sqrt((tested.x - left.x)*(tested.x - left.x) + (tested.y - left.y)*(tested.y - left.y));
			//float PB = sqrt((right.x - tested.x)*(right.x - tested.x) + (right.y - tested.y)*(right.y - tested.y));
			//if (AB == AP + PB)
			//	return (*it);


			float tmp1 = (float)( tested.x - left.x ) / (right.x - left.x);
			float tmp2 = (float)(tested.y - left.y) / (right.y - left.y);
			if (abs(tmp1 - tmp2) < 0.02 && (*it)->GetLabel() != label) // point belongs to line
			{
				Line* current = (*it);
				above = new Line((*current));
				return above;
			}
		}
	}

	return above;
}

Line* PriorityQueue::Below(Point p, list<Line*> line_state, int lowerEdge, string label)
{
	Line* below = NULL;

	for (int y = p.y; y <= lowerEdge; y++)
	{
		Point tested = Point(p.x, y);
		for (list<Line*>::iterator it = line_state.begin(); it != line_state.end(); it++)
		{

			Point left = (*it)->GetLeftPoint();
			Point right = (*it)->GetRightPoint();

			float tmp1 = (float)(tested.x - left.x) / (right.x - left.x);
			float tmp2 = (float)(tested.y - left.y) / (right.y - left.y);
			if (abs(tmp1 - tmp2) < 0.02 && (*it)->GetLabel() != label) // point belongs to line
			{
				Line* current = (*it);
				below = new Line((*current));
				return below;
			}
		}
	}

	return below;
}

bool PriorityQueue::Member(OrderItem* item)
{
	bool contains = false;
	list<OrderItem*>::iterator it = this->orderItems.begin();
	for (it; it != this->orderItems.end(); it++)
	{
		if ((*it)->GetPoint().x == item->GetPoint().x && (*it)->GetPoint().y == item->GetPoint().y)
		{
			contains = true;
		}
	}
	return contains;
}

OrderItem* PriorityQueue::MIN_Elem()
{
	OrderItem* item = new OrderItem((*this->orderItems.front()));
	this->orderItems.remove(this->orderItems.front());
	return item;
}

list<OrderItem*> PriorityQueue::GetListItems()
{
	return this->orderItems;
}

void PriorityQueue::SortElements()
{
	this->orderItems.sort([](OrderItem* item1, OrderItem* item2) {
		return item1->GetPoint().x < item2->GetPoint().x;
	});
}
