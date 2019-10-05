#include <iostream>
#include "pch.h"
//opencv - https://opencv.org/
#include <opencv2/opencv.hpp>
#include "Models.h";

using namespace std;
using namespace cv; 


PriorityQueue InsertPointIntoPriorityQueue(list<Line> lines);
void InsertPointsIntoPriorityQueue(PriorityQueue* queue,list<Line> lines);
void Swap_S1_S2_InAlpha(Line* s1, Line* s2, list<Line*>& alpha);
void DeleteItemInAlpha(Line* line, list<Line*>& alpha);
OrderItem* FindIntersection(Line* s1, Line* s2);
void StartPlaneSweepAlgorithm(list<Point>& intersections, PriorityQueue* queue, int lowerEdge);
list<Line> CreateTestData();
int GetMostRightPoint(list<Line> lines);
int GetMostLowerPoint(list<Line> lines);
void ShoeResults(list<Line> lines, list<Point> intersections);

// ************************

list<Line> CreateTestDataToFillpolygon();
void InsertPointsIntoPriorityQueueForPolygon(PriorityQueue* queue, list<Line> lines);
void PlaneSweepWithFillpolygon();
void StartFillPolygon(list<Line>& intersections, PriorityQueue* queue, int lowerEdge, int rightEdge);
void UpdateAlpha(list<Line*>& alpha, list<OrderItem*>& points, int current_y);
void AddLines(list<Line>& addedLines, list<Line*> alpha, int current_y, int rightEdge);

int main(int argc, char** argv)
{      
	//list<Line> data = CreateTestData();
	//PriorityQueue* queue = new PriorityQueue();
	//InsertPointsIntoPriorityQueue(queue,data);
	//queue->SortElements();
	//list<Point> intersections;
	//StartPlaneSweepAlgorithm(intersections, queue, GetMostLowerPoint(data));
	//ShoeResults(data, intersections);

	list<Line> data = CreateTestDataToFillpolygon();
	list<Point> points;
	ShoeResults(data, points);
	PriorityQueue* queue = new PriorityQueue();
	InsertPointsIntoPriorityQueueForPolygon(queue, data);
	list<Line> addedLines;
	StartFillPolygon(addedLines, queue, GetMostLowerPoint(data),GetMostRightPoint(data));
	
	
	data.splice(data.end(), addedLines);
	ShoeResults(data, points);

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

void InsertPointsIntoPriorityQueue(PriorityQueue* eventQueue,list<Line> lines)
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
	Line l = Line("s1",25,33,272,157, Scalar(0, 255, 255));
	Line l2 = Line("s2", 62, 88, 211, 50, Scalar(0, 255, 255));
	Line l3 = Line("s3", 125, 136, 421, 87, Scalar(0, 255, 255));
	Line l4 = Line("s4", 168, 26, 315, 154, Scalar(0, 255, 255));
	result.push_back(l);
	result.push_back(l2);
	result.push_back(l3);
	result.push_back(l4);

	return result;
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
	//float dx = s1->GetRightPoint().x - s1->GetLeftPoint().x;
	//float dy = s1->GetRightPoint().y - s1->GetLeftPoint().y;
	//float m1 = dy / dx;

	//// y = mx + c
	//// intercept c = y - mx
	//float c1 = s1->GetLeftPoint().y - m1 * s1->GetLeftPoint().x; // which is same as y2 - slope * x2

	//dx = s2->GetRightPoint().x - s2->GetLeftPoint().x;
	//dy = s2->GetRightPoint().y - s2->GetLeftPoint().y;
	//float m2 = dy / dx;
	//float c2 = s2->GetRightPoint().y - m2 * s2->GetRightPoint().x; // which is same as y2 - slope * x2



	//std::cout << "Equation of line1: ";
	//std::cout << m1 << "X " << ((c1 < 0) ? ' ' : '+') << c1 << "\n";
	//std::cout << "Equation of line2: ";
	//std::cout << m2 << "X " << ((c2 < 0) ? ' ' : '+') << c2 << "\n";



	//if ((m1 - m2) == 0)
	//{
	//	std::cout << "No Intersection between the lines\n";
	//	return NULL;
	//}
	//else
	//{
	//	float intersection_X = (c2 - c1) / (m1 - m2);

	//	float intersection_Y = m1 * intersection_X + c1;

	//	std::cout << "Intersecting Point: = ";
	//	std::cout << intersection_X;
	//	std::cout << ",";
	//	std::cout << intersection_Y;
	//	std::cout << "\n";

	//	OrderItem* result = new OrderItem(Point(intersection_X,intersection_Y),NULL, INTERSECTION);
	//	// intersections lines must be in order s1 above s2 by y coordinate
	//	if (s1->GetLeftPoint().y < s2->GetLeftPoint().y)
	//	{
	//		result->SetIntersectionLines(s1, s2);
	//	}
	//	else
	//	{
	//		result->SetIntersectionLines(s2, s1);
	//	}
	//}

	// Line AB represented as a1x + b1y = c1 
	double a1 = s1->GetRightPoint().y - s1->GetLeftPoint().y;
	double b1 = s1->GetLeftPoint().x - s1->GetRightPoint().x;
	double c1 = a1 * (s1->GetLeftPoint().x) + b1 * (s1->GetLeftPoint().y);


	// Line CD represented as a2x + b2y = c2 
	double a2 = s2->GetRightPoint().y - s2->GetLeftPoint().y;
	double b2 = s2->GetLeftPoint().x - s2->GetRightPoint().x;
	double c2 = a2 * (s2->GetLeftPoint().x) + b2 * (s2->GetLeftPoint().y);


	double determinant = a1 * b2 - a2 * b1;

	if (determinant == 0)
	{
		// The lines are parallel. This is simplified 
		// by returning a pair of FLT_MAX 
		return NULL;
	}
	else
	{
		double x = (b2*c1 - b1 * c2) / determinant;
		double y = (a1*c2 - a2 * c1) / determinant;
		OrderItem* result = new OrderItem(Point(x,y),NULL, INTERSECTION);
		// intersections lines must be in order s1 above s2 by y coordinate
		if (s1->GetLeftPoint().y < s2->GetLeftPoint().y)
		{
			result->SetIntersectionLines(s1, s2);
		}
		else
		{
			result->SetIntersectionLines(s2, s1);
		}
		return result;
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

	Mat img = Mat::zeros(GetMostLowerPoint(lines)+20, GetMostRightPoint(lines)+20, CV_8UC3);
	for (it; it != lines.end(); it++)
	{
		line(img,it->GetLeftPoint(),it->GetRightPoint(),it->GetColor());
	}

	list<Point>::iterator p = intersections.begin();

	for (p; p != intersections.end(); p++)
	{
		circle(img, (*p), 4, Scalar(255, 255, 255), FILLED);
	}

	imshow("Plane sweep algorithm", img);
	waitKey(0);
}

// ************ FILL POLYGON USING PLANE SWEEP ALGORITHM ******************

list<Line> CreateTestDataToFillpolygon() 
{
	list<Line> result;
	result.push_back(Line("s1", 145, 59, 228, 59, Scalar(0, 255, 255)));
	result.push_back(Line("s2", 228, 59, 228, 96, Scalar(0, 255, 255)));
	result.push_back(Line("s3", 228, 96, 200, 136, Scalar(0, 255, 255)));
	result.push_back(Line("s4", 200, 136, 177, 93, Scalar(0, 255, 255)));
	result.push_back(Line("s5", 177, 93, 160, 93, Scalar(0, 255, 255)));
	result.push_back(Line("s6", 160, 93, 160, 189, Scalar(0, 255, 255)));
	result.push_back(Line("s7", 160, 189, 264, 189, Scalar(0, 255, 255)));
	result.push_back(Line("s8", 264, 189, 264, 159, Scalar(0, 255, 255)));
	result.push_back(Line("s9", 264, 159, 338, 159, Scalar(0, 255, 255)));
	result.push_back(Line("s10", 338, 159, 338, 244, Scalar(0, 255, 255)));
	result.push_back(Line("s11", 338, 244, 145, 244, Scalar(0, 255, 255)));
	result.push_back(Line("s12", 145, 244, 145, 59, Scalar(0, 255, 255)));


	return result;
}

void InsertPointsIntoPriorityQueueForPolygon(PriorityQueue* queue, list<Line> lines)
{
	int mostLower = GetMostLowerPoint(lines);
	Line* foundLine = NULL;
	list<Line>::iterator it;

	for (it = lines.begin(); it != lines.end(); it++)
	{
		OrderItem* item = new OrderItem((*it).GetLeftPoint(),new Line((*it)),LEFT);
		queue->Insert(item);
		OrderItem* item2 = new OrderItem((*it).GetRightPoint(), new Line((*it)), LEFT);
		queue->Insert(item2);
	}

	queue->SortForPolygon();
}

void StartFillPolygon(list<Line>& addedLines, PriorityQueue* queue, int lowerEdge, int rightEdge)
{
	// we are going verticaly from top to bottom
	int step = 5;
	list<Line*> alpha;
	int current_y = step;
	list<OrderItem*> points;

	while (queue->GetElementsForPolygon(&points)) {}
	//list<OrderItem*> points = queue->GetElementForPolygon();
	current_y = points.front()->GetPoint().y +step;

	while (!queue->GetListItems().empty())
	{
		OrderItem* p = queue->MIN_Elem();
		if (points.empty())
		{
			points.push_back(p);
			continue;
		}
		else
		{
			if (p->GetPoint().y == points.back()->GetPoint().y)
			{
				points.push_back(p);
				continue;
			}
			else
			{
				// remove and add lines to the imaginary line sweep state queue
				UpdateAlpha(alpha,points, current_y);

				while (current_y < p->GetPoint().y)
				{
					//add horizontal lines between vertical lines
					AddLines(addedLines, alpha, current_y,rightEdge+30);

					current_y += step;
					UpdateAlpha(alpha, points, current_y);

				}

				points.push_back(p);
			}

		}

	}
}

void UpdateAlpha(list<Line*>& alpha, list<OrderItem*>& points, int current_y)
{
	//remove lines that are whole above sweep line
	list<Line*>::iterator line_it;
	Line* current = NULL;
	for (line_it = alpha.begin(); line_it != alpha.end();)
	{
		current = (*line_it);
		if (current->GetLeftPoint().y < current_y && current->GetRightPoint().y < current_y)
		{
			line_it = alpha.erase(line_it);
			continue;
		}
		line_it++;
	}


	list<OrderItem*>::iterator it;
	for (it = points.begin(); it != points.end();)
	{
		Line* current = (*it)->GetCurrentLine();
		if (current->GetLeftPoint().y <= current_y && current->GetRightPoint().y <= current_y)
		{
			// whole line is above sweep line
			it = points.erase(it);
			continue;
		}

		if (current->GetLeftPoint().y <= current_y && current->GetRightPoint().y >= current_y)
		{
			list<Line*>::iterator exists = find_if(alpha.begin(), alpha.end(), [current](Line* l) {
				return (bool)(l->GetLabel() == current->GetLabel());
			});
			if (exists == alpha.end())
			{
				alpha.push_back(new Line((*current)));
			}

		}

		if (current->GetLeftPoint().y >= current_y && current->GetRightPoint().y <= current_y)
		{
			list<Line*>::iterator exists = find_if(alpha.begin(), alpha.end(), [current](Line* l) {
				return (l->GetLabel() == current->GetLabel());
			});
			if (exists == alpha.end())
			{
				alpha.push_back(new Line((*current)));
			}
		}



		it++;
	}
}

void AddLines(list<Line>& addedLines, list<Line*> alpha, int current_y, int rightEdge)
{
	
	Line* horizontal = new Line("horizontal",0,current_y,rightEdge,current_y, Scalar(0, 0, 255));
	list<Point*> intersections;
	list<Line*>::iterator it;
	for (it = alpha.begin(); it != alpha.end(); it++)
	{
		Line* current = (*it);
		Point* p = new Point(FindIntersection(horizontal, current)->GetPoint());
		intersections.push_back(p);
	}

	//sort by x
	intersections.sort([](Point* item1, Point* item2) {
		return item1->x < item2->x;
	});

	// at this point there should be even count of intersections
	list<Point*>::iterator point_it;
 	for (point_it = intersections.begin(); point_it != intersections.end(); point_it++)
	{
		Point* first = (*point_it);
		point_it++;
		Point* second = (*point_it);

		addedLines.push_back(Line("Line" + current_y, first->x, first->y, second->x, second->y, Scalar(0, 0, 255)));
	}

	intersections.clear();
}