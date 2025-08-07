#pragma once
#include <SDL.h>
#include <vector>
#include <memory>
#include <array>

#include "utils.h"
#include "PrettyColors.h"


class Time;
class Texture;

class Pathfinding
{

public:
    Pathfinding(int nodesPerRow);
    Pathfinding(const Pathfinding& gol) = delete;
    Pathfinding& operator=(const Pathfinding& gol) = delete;
    Pathfinding(Pathfinding&& gol) = delete;
    Pathfinding& operator=(Pathfinding&& gol) = delete;

    ~Pathfinding();
    void SetEdgeNodes();

    void Draw() const;
    void DrawMiddlePoint() const;
    void DrawNodes() const;
    void DrawGrid() const;

    void DrawUI(Rectf viewport) const;
    void DrawStatePicker(Rectf viewport) const;
    void DrawStatePickerText(Rectf statePickerPos,bool draw) const;
    void DrawPausedText(Rectf viewport) const;

    void Update(float elapsedTime);
    void UpdateTextures();
    void UpdateStatePickerText();
    void UpdatePausedText();

    void AStarAlgorithm();
    int CalculateDistanceBetweenNodes(int firstId,int secondId) const; // One step is 10. Or 14 diagonally
    bool OneStartAndEndPoint() const;

    // Returns Node.Id or -1 when not overlapping 
    int NodeOverlapping(Point2f target)const;

    // Same as Zoom
    void Start();
    void Pause();
    void FullRestart();
    void RestartAStar();

    // NodeStatePicker
    void CycleStateUp();
    void CycleStateDown();

    // Zooming need to go into a separate class
    void ZoomIn(Point2f mousePos);
    void ZoomOut(Point2f mousePos);
    void ResetZoom();
    float GetScale() const;
    Point2f GetCenterOfGrid() const;
    //

    void OnMouseUp(const SDL_MouseButtonEvent& e);
    void OnMouseDown(const SDL_MouseButtonEvent& e);
    void OnMouseMotion(const SDL_MouseMotionEvent& e);


private:

    // https://youtu.be/-L-WgKMFuhE?si=Y8M0LN_eo3Bftx1X&t=453 Algorithm explanation

    // Game basics:

    enum class AStarState
    {
        NoList = 0,
        OpenList = 1,
        ClosedList = 2
    };
    enum class NodeState
    {
        empty = 0,
        bridge = 1,
        water = 2,
        rock = 3,
        start = 4,
        end = 5,

        TotalStates
    }; // Should have some variable like "Traversable" instead of having multiple Traversable states checked
    struct Node
    {
        int id;
        AStarState list;
        NodeState state;
        float gCost; // Distance from parent + parent's gCost (gCost is the distance from starting Node)
        float hCost; // Distance from end node
        float fCost; // gCost + hCost
        int parentId;
        bool drawnTo; // In current mouse swipe
        bool edge;
        bool partOfPath;
    };
    const int m_NodesPerRow;
    const float m_NodeSize;
    std::vector<Node> m_Nodes;
    float m_DefaultNodeCosts;
    std::array<int,8> m_NeighborsOffsets; // 8 being all 8 nodes around the target

    // StatePicker
    NodeState m_StatePicked;
    bool m_StatePickedChanged;
    std::unique_ptr<Texture> m_StatePickerText;

    // AStar
    std::vector<int> m_OpenList;
    std::vector<int> m_ClosedList;
    bool m_EndFound;
    bool m_PathSet;
    bool m_NoPathPossible;

    // Drawing
    Point2f m_GridPos;
    float m_LineWidth;

    // Mouse
    Point2f m_LastMousePos;
    std::unique_ptr<Time> m_LBHeldTimer;

    // Time (Start, Pause, Restart...)
    std::unique_ptr<Texture> m_TimeState;
    bool m_TimeStateChanged;
    std::unique_ptr<Time> m_Time;
    bool m_Paused;

    // Zooming
    Point2f m_ZoomTarget;
    float m_ScaleFactor;

    // Counter to make certain things happen less than 1 time per frame
    int m_FrameCounter{0};

};

