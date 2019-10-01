#include <iostream>
#include "pch.h"
//opencv - https://opencv.org/
#include <opencv2/opencv.hpp>
#include "Models.h";

using namespace std;
using namespace cv; 


PriorityQueue InsertPointIntoPriorityQueue(list<Line> lines);
void InsertPointsIntoPriorityQueue2(PriorityQueue* queue,list<Line> lines);
void Swap_S1_S2_InAlpha(Line* s1, Line* s2, list<Line*>& alpha);
void DeleteItemInAlpha(Line* line, list<Line*>& alpha);
OrderItem* FindIntersection(Line* s1, Line* s2);
void StartPlaneSweepAlgorithm(list<Point>& intersections, PriorityQueue* queue, int lowerEdge);
list<Line> CreateTestData();
int GetMostRightPoint(list<Line> lines);
int GetMostLowerPoint(list<Line> lines);

void ShoeResults(list<Line> lines, list<Point> intersections);

int main(int argc, char** argv)
{      
	list<Line> data = CreateTestData();
	PriorityQueue* queue = new PriorityQueue();
	InsertPointsIntoPriorityQueue2(queue,data);
	queue->SortElements();
	list<Point> intersections;
	StartPlaneSweepAlgorithm(intersections, queue, GetMostLowerPoint(data));
	ShoeResults(data, intersections);
}

void StartPlaneSweepAlgorithm(list<Point>& intersection_points, PriorityQueue* queue, int lowerEdge)
{
	list<OrderItem*> intersections;
	list<Line*> alpha;
	while (!queue->GetListItems().empty())
	{
		OrderItem* p = queue->MIN_Elem();
		if (p->Type == LEFT)
		{
			Line* s = p->GetCurrentLine();
			alpha.push_back(s);
			Line* s1 = queue->Above(p->GetPoint(),alpha, s->GetLabel());
			Line* s2 = queue->Below(p->GetPoint(),alpha, lowerEdge, s->GetLabel());
			if (s1 != NULL) // check intersection with s1
			{
				OrderItem* common = FindIntersection(s, s1);
				if (common != NULL)
				{
					intersections.push_back(common);
				}
			}
			if (s2 != NULL)// check intersection with s2
			{
				OrderItem* common = FindIntersection(s, s2);
				if (common != NULL)
				{
					intersections.push_back(common);
				}
			}
		}
		else if (p->Type == RIGHT)
		{
			Line* s = p->GetCurrentLine();
			Line* s1 = queue->Above(p->GetPoint(), alpha, s->GetLabel());
			Line* s2 = queue->Below(p->GetPoint(), alpha, lowerEdge, s->GetLabel());
			if (s1 != NULL && s2 != NULL) // check intersection between s1 and s2
			{
				OrderItem* common = FindIntersection(s1, s2);
				if (common != NULL)
				{
					intersections.push_back(common);
				}
			}
			DeleteItemInAlpha(s, alpha);

		}
		else //intersection point
		{
			Line* s1 = p->GetIntersectionLineS1();
			Line* s2 = p->GetIntersectionLineS2();
			Line* s3 = queue->Above(s1->GetLeftPoint(), alpha,s1->GetLabel());
			Line* s4 = queue->Below(s2->GetLeftPoint(), alpha, lowerEdge, s2->GetLabel());

			if (s3 != NULL && s2 != NULL) // check intersection between s3 and s2
			{
				OrderItem* common = FindIntersection(s3, s2);
				if (common != NULL)
				{
					intersections.push_back(common);
				}
			}
			if (s1 != NULL && s4 != NULL)// check intersection between s1 and s4
			{
				OrderItem* common = FindIntersection(s1, s4);
				if (common != NULL)
				{
					intersections.push_back(common);
				}
			}

			Swap_S1_S2_InAlpha(s1, s2, alpha);
		}

		list<OrderItem*>::iterator c = intersections.begin();
		for (c; c != intersections.end(); c++)
		{
			intersection_points.push_back((*c)->GetPoint());
			if (!queue->Member((*c)))
			{
				queue->Insert((*c));
			}
		}
		queue->SortElements();
		intersections.clear();
	}
}

void InsertPointsIntoPriorityQueue2(PriorityQueue* eventQueue,list<Line> lines)
{

	int mostRight_X = GetMostRightPoint(lines);

	Line* foundLine = NULL;
	list<Line>::iterator it;
	for (int x = 0; x <= mostRight_X; x++)
	{
		for (it = lines.begin(); it != lines.end(); it++)
		{
			if (x == (*it).GetLeftPoint().x || x == (*it).GetRightPoint().x)
			{
				foundLine = &(*it);
				break;
			}
		}

		if (foundLine == NULL)
			continue;
		
		if (foundLine->GetLeftPoint().x == x)
		{
			OrderItem* item = new OrderItem(foundLine->GetLeftPoint(),new Line((*foundLine)),LEFT);
			eventQueue->Insert(item);
		}
		else
		{
			OrderItem* item = new OrderItem(foundLine->GetRightPoint(), new Line((*foundLine)), RIGHT);
			eventQueue->Insert(item);
		}
		foundLine = NULL;
	}

}

list<Line> CreateTestData()
{
	list<Line> result;
	Line l = Line("s1",25,33,272,157);
	Line l2 = Line("s2", 62, 88, 211, 50);
	Line l3 = Line("s3", 125, 136, 421, 87);
	Line l4 = Line("s4", 168, 26, 315, 154);
	result.push_back(l);
	result.push_back(l2);
	result.push_back(l3);
	result.push_back(l4);

	return result;
}

PriorityQueue InsertPointsIntoPriorityQueue(list<Line> lines)
{
	PriorityQueue eventQueue = PriorityQueue();

	//int mostRight_X = GetMostRightPoint(lines);

	//lines.sort();

	//list<OrderItem*>::iterator eventIt;
	//Line* foundLine;
	//OrderItem* aboveItem;
	//OrderItem* belowItem;

	//list<Line>::iterator it = lines.begin();
	//for (int x = 0; x < mostRight_X; x++)
	//{
	//	for (it = lines.begin(); it != lines.end(); it++)
	//	{
	//		if (x == (*it).GetLeftPoint().x || x == (*it).GetLeftPoint().x)
	//		{
	//			foundLine = &(*it);
	//			break;
	//		}
	//	}

	//
	//	if (foundLine != NULL)
	//	{

	//		OrderItem* item;
	//		//found line with point on that x position
	//		if (x == foundLine->GetLeftPoint().x)
	//		{
	//			for (eventIt = eventQueue.GetListItems().begin(); eventIt != eventQueue.GetListItems().end(); eventIt++)
	//			{
	//				OrderItem* current = (*eventIt);
	//				if (current->GetCurrentLine()->GetLeftPoint().x < x && current->GetCurrentLine()->GetRightPoint().x > x) // lines that are crosing trough vertical x line
	//				{
	//					if (current->GetPoint().y < foundLine->GetLeftPoint().y) //it's above line
	//					{
	//						if (aboveItem != NULL)
	//						{
	//							if (aboveItem->GetPoint().y < current->GetPoint().y)
	//							{
	//								aboveItem = current;
	//							}
	//						}
	//						else
	//						{
	//							aboveItem = current;
	//						}
	//					}
	//					else // it's below line
	//					{
	//						if (belowItem != NULL)
	//						{
	//							if (belowItem->GetPoint().y > current->GetPoint().y)
	//							{
	//								belowItem = current;
	//							}
	//						}
	//						else
	//						{
	//							belowItem = current;
	//						}
	//					}
	//				}

	//			}

	//			item = new OrderItem(foundLine->GetLeftPoint(), foundLine,true);
	//			item->SetAbove(aboveItem);
	//			item->SetBelow(belowItem);
	//		}
	//		else
	//		{
	//			for (eventIt = eventQueue.GetListItems().begin(); eventIt != eventQueue.GetListItems().end(); eventIt++)
	//			{
	//				OrderItem* current = (*eventIt);
	//				if (current->GetCurrentLine()->GetLeftPoint().x < x && current->GetCurrentLine()->GetRightPoint().x > x) // lines that are crosing trough vertical x line
	//				{
	//					if (current->GetPoint().y < foundLine->GetLeftPoint().y) //it's above line
	//					{
	//						if (aboveItem != NULL)
	//						{
	//							if (aboveItem->GetPoint().y < current->GetPoint().y)
	//							{
	//								aboveItem = current;
	//							}
	//						}
	//						else
	//						{
	//							aboveItem = current;
	//						}
	//					}
	//					else // it's below line
	//					{
	//						if (belowItem != NULL)
	//						{
	//							if (belowItem->GetPoint().y > current->GetPoint().y)
	//							{
	//								belowItem = current;
	//							}
	//						}
	//						else
	//						{
	//							belowItem = current;
	//						}
	//					}
	//				}

	//			}
	//			item = new OrderItem(foundLine->GetRightPoint(), foundLine, false);
	//			item->SetAbove(aboveItem);
	//			item->SetBelow(belowItem);
	//		}

	//		eventQueue.Insert(item);

	//	}
	//}	
	return eventQueue;
}

int GetMostRightPoint(list<Line> lines)
{
	int right = 0;
	list<Line>::iterator it = lines.begin();
	for (it; it != lines.end(); it++)
	{
		if ((*it).GetRightPoint().x > right)
		{
			right = (*it).GetRightPoint().x;
		}
	}

	return right+5;
}

int GetMostLowerPoint(list<Line> lines)
{
	int lower = 0;
	list<Line>::iterator it = lines.begin();
	for (it; it != lines.end(); it++)
	{
		if ((*it).GetRightPoint().y > lower)
		{
			lower = (*it).GetRightPoint().y;
		}
		if ((*it).GetLeftPoint().y > lower)
		{
			lower = (*it).GetLeftPoint().y;
		}
	}

	return lower + 2;
}

OrderItem* FindIntersection(Line* s1, Line* s2)
{
	float dx = s1->GetRightPoint().x - s1->GetLeftPoint().x;
	float dy = s1->GetRightPoint().y - s1->GetLeftPoint().y;
	float m1 = dy / dx;

	// y = mx + c
	// intercept c = y - mx
	float c1 = s1->GetLeftPoint().y - m1 * s1->GetLeftPoint().x; // which is same as y2 - slope * x2

	dx = s2->GetRightPoint().x - s2->GetLeftPoint().x;
	dy = s2->GetRightPoint().y - s2->GetLeftPoint().y;
	float m2 = dy / dx;
	float c2 = s2->GetRightPoint().y - m2 * s2->GetRightPoint().x; // which is same as y2 - slope * x2



	std::cout << "Equation of line1: ";
	std::cout << m1 << "X " << ((c1 < 0) ? ' ' : '+') << c1 << "\n";
	std::cout << "Equation of line2: ";
	std::cout << m2 << "X " << ((c2 < 0) ? ' ' : '+') << c2 << "\n";



	if ((m1 - m2) == 0)
	{
		std::cout << "No Intersection between the lines\n";
		return NULL;
	}
	else
	{
		float intersection_X = (c2 - c1) / (m1 - m2);

		float intersection_Y = m1 * intersection_X + c1;

		std::cout << "Intersecting Point: = ";
		std::cout << intersection_X;
		std::cout << ",";
		std::cout << intersection_Y;
		std::cout << "\n";

		OrderItem* result = new OrderItem(Point(intersection_X,intersection_Y),NULL, INTERSECTION);
		// intersections lines must be in order s1 above s2 by y coordinate
		if (s1->GetLeftPoint().y < s2->GetLeftPoint().y)
		{
			result->SetIntersectionLines(s1, s2);
		}
		else
		{
			result->SetIntersectionLines(s2, s1);
		}
	}
}

void Swap_S1_S2_InAlpha(Line* s1, Line* s2, list<Line*>& alpha)
{
	list<Line*> new_alpha;
	list<Line*>::iterator it_s1 = find_if(alpha.begin(), alpha.end(), [s1](Line* l) {
		return (s1->GetLeftPoint().x == l->GetLeftPoint().x && s1->GetLeftPoint().y == l->GetLeftPoint().y
			&& s1->GetRightPoint().x == l->GetRightPoint().x && s1->GetRightPoint().y == l->GetRightPoint().y);
	});
	list<Line*>::iterator it_s2 = find_if(alpha.begin(), alpha.end(), [s2](Line* l) {
		return (s2->GetLeftPoint().x == l->GetLeftPoint().x && s2->GetLeftPoint().y == l->GetLeftPoint().y
			&& s2->GetRightPoint().x == l->GetRightPoint().x && s2->GetRightPoint().y == l->GetRightPoint().y);
	});

	int index_s1 = distance(alpha.begin(), it_s1);
	int index_s2 = distance(alpha.begin(), it_s2);

	int index;
	list<Line*>::iterator it = alpha.begin();
	for (it, index = 0; it != alpha.end(); it++, index++)
	{
		if (index == index_s1)
		{
			Line* l = new Line();
			l = (*it_s2);
			new_alpha.push_back(l);
		}
		else if (index == index_s2)
		{
			Line* l = new Line();
			l = (*it_s1);
			new_alpha.push_back(l);
		}
		else
		{
			new_alpha.push_back((*it));
		}
		
	}

	alpha = new_alpha;
}

void DeleteItemInAlpha(Line* line, list<Line*>& alpha)
{
	list<Line*>::iterator it = find_if(alpha.begin(), alpha.end(), [line](Line* l) {
		return l->GetLabel() == line->GetLabel();
	});
	Line* toDelete = (*it);
	alpha.remove(toDelete);

}

void ShoeResults(list<Line> lines, list<Point> intersections)
{
	list<Line>::iterator it = lines.begin();

	Mat img = Mat::zeros(GetMostLowerPoint(lines), GetMostRightPoint(lines), CV_8UC3);
	for (it; it != lines.end(); it++)
	{
		line(img,it->GetLeftPoint(),it->GetRightPoint(),Scalar(0, 255, 255));
	}

	list<Point>::iterator p = intersections.begin();

	for (p; p != intersections.end(); p++)
	{
		circle(img, (*p), 4, Scalar(255, 255, 255), FILLED);
	}

	imshow("Plane sweep algorithm", img);
	waitKey(0);
}