#pragma once
using namespace System;

public ref class RubberBandSelection
{
private:
    System::Drawing::Point startPoint;
    System::Drawing::Point endPoint;
    bool isSelecting;

public:
    RubberBandSelection()
    {
        isSelecting = false;
    }

    void BeginSelection(System::Drawing::Point start)
    {
        startPoint = start;
        isSelecting = true;
    }

    void UpdateSelection(System::Drawing::Point current)
    {
        if (isSelecting)
            endPoint = current;
    }

    void EndSelection(System::Drawing::Point end)
    {
        endPoint = end;
        isSelecting = false;
    }

    property System::Drawing::Point StartPoint
    {
        System::Drawing::Point get() { return startPoint; }
    }

    property System::Drawing::Point EndPoint
    {
        System::Drawing::Point get() { return endPoint; }
    }

    property bool IsSelecting
    {
        bool get() { return isSelecting; }
    }

    System::Drawing::Rectangle GetSelectionRectangle()
    {
        int x = Math::Min(startPoint.X, endPoint.X);
        int y = Math::Min(startPoint.Y, endPoint.Y);
        int width = Math::Abs(startPoint.X - endPoint.X);
        int height = Math::Abs(startPoint.Y - endPoint.Y);
        return System::Drawing::Rectangle(x, y, width, height);
    }
};