#include <cmath>
#include <iostream>
#include <print>

#include "Pathfinding.h"
#include "Time.h"
#include "Texture.h"
#include "OrientationManager.h"


Pathfinding::Pathfinding(int nodesPerRow)
    :m_NodesPerRow{nodesPerRow}
    ,m_NodeSize{15.0f}
    ,m_DefaultNodeCosts{10000.0f}
    ,m_StatePicked{NodeState::empty}
    ,m_StatePickedChanged{false}
    ,m_EndFound{false}
    ,m_PathSet{false}
    ,m_NoPathPossible{false}
    ,m_GridPos{20.0f,20.0f}
    ,m_LineWidth{1.0f}
    ,m_LastMousePos{0.0f,0.0f}
    ,m_LBHeldTimer{std::make_unique<Time>(0.1f)}
    ,m_TimeStateChanged{false}
    ,m_Time{std::make_unique<Time>(0.0f)}
    ,m_Paused{true}
    ,m_ZoomTarget{0.0f,0.0f}
    ,m_ScaleFactor{1.0f}
    ,m_NeighborsOffsets{-1,1
        ,-m_NodesPerRow + 1,-m_NodesPerRow - 1
        ,m_NodesPerRow,-m_NodesPerRow
        ,m_NodesPerRow + 1,m_NodesPerRow - 1}
{
    m_LBHeldTimer->Pause();
    // SDL/OpenGl line width limit?
    if (m_LineWidth > 10.0f)
    {
        m_LineWidth = 10.0f;
    }

    using enum NodeState;
    using enum AStarState;
    // Reserve space for vector? Should apparently first check if it's actually better performence wise, but with such big size it should be
    m_Nodes.reserve(Uint64(pow(m_NodesPerRow,2)));
    for (int i{0}; i < pow(m_NodesPerRow,2); i++)
    {
        m_Nodes.push_back(Node{i, NoList, empty, m_DefaultNodeCosts, m_DefaultNodeCosts,m_DefaultNodeCosts, i, false, false, false});
    }

    SetEdgeNodes();

    // Textures
    m_StatePickerText = std::make_unique<Texture>("Empty: ","Resources/Fonts/consola.ttf",24,PrettyColors::GetColor(PrettyColors::white));
    m_TimeState = std::make_unique<Texture>("Paused","Resources/Fonts/consola.ttf",24,PrettyColors::GetColor(PrettyColors::white));
}

Pathfinding::~Pathfinding()
{
    m_Nodes.clear();
}

void Pathfinding::SetEdgeNodes()
{
    for (int i{0}; i < m_Nodes.size(); i++)
    {
        if (i < m_NodesPerRow ||
            i > m_Nodes.size() - m_NodesPerRow - 1 ||
            i % m_NodesPerRow == m_NodesPerRow - 1 ||
            i % m_NodesPerRow == 0)
        {
            m_Nodes[i].edge = true;
            m_Nodes[i].state = NodeState::rock;
        }
    }
}

void Pathfinding::ZoomIn(Point2f mousePos)
{
    m_ScaleFactor += 0.1f;
    m_ZoomTarget = mousePos;
}
void Pathfinding::ZoomOut(Point2f mousePos)
{
    m_ScaleFactor -= 0.1f;
    if (m_ScaleFactor < 0.1f)
    {
        m_ScaleFactor = 0.1f;
    }
    m_ZoomTarget = mousePos;

}
void Pathfinding::ResetZoom()
{
    m_ScaleFactor = 1.0f;
}

void Pathfinding::Draw() const
{
    glPushMatrix();

    glTranslatef(m_GridPos.x
        ,m_GridPos.y
        ,0.0f);

    // No zoom for now
    // glScalef(m_ScaleFactor,m_ScaleFactor,0);

    DrawNodes();
    DrawMiddlePoint();

    glPopMatrix();
}
void Pathfinding::DrawGrid() const
{
  // Useless???
}
void Pathfinding::DrawMiddlePoint() const
{
    using namespace PrettyColors;
    using namespace utils;

    float centerPointSize{6.0f};
    SetColor(GetColor(red));
    DrawPoint(GetCenterOfGrid().x,
        GetCenterOfGrid().y,
        centerPointSize);

}
void Pathfinding::DrawNodes() const
{
    using namespace std;
    using namespace utils;
    using namespace PrettyColors;
    using enum NodeState;
    using enum AStarState;

    float widthOffset{m_LineWidth / 2.0f};
    bool drawOutlines{true};
    int indexCounter{0};

    for (int i{0}; i < m_Nodes.size(); i++)
    {
        int row = i / m_NodesPerRow;
        int col = i % m_NodesPerRow;

        // Sets Main colors
        switch (m_Nodes[i].state)
        {
        case empty:
            SetColor(GetColor(white));
            break;
        case bridge:
            SetColor(GetColor(brown));
            break;
        case water:
            SetColor(GetColor(blue));
            break;
        case rock:
            SetColor(GetColor(gray));
            break;
        }

        // Checked nodes
        switch (m_Nodes[i].list)
        {
        case OpenList:
            SetColor(GetColor(lGreen));
            break;
        case ClosedList:
            SetColor(GetColor(lRed));
            break;
        default:

            break;
        }

        switch (m_Nodes[i].partOfPath)
        {
        case true:
            SetColor(GetColor(cyan));
            break;
        case false:

            break;
        }

        // To avoid start/end point re-color
        switch (m_Nodes[i].state)
        {
        case end:
            SetColor(GetColor(dRed));
            break;
        case start:
            SetColor(GetColor(dGreen));
            break;
        }


      FillRect(col * (m_NodeSize)
          ,row * (m_NodeSize)
          ,m_NodeSize,m_NodeSize);

      // DrawOutlines:  
      if (drawOutlines == false)
          continue;

      SetColor(GetColor(black));
      DrawRect(col * (m_NodeSize)
          ,row * (m_NodeSize)
          ,m_NodeSize,m_NodeSize);
    }

}
void Pathfinding::DrawUI(Rectf viewport) const
{
    //m_PopulationTxt->Draw(Point2f{10.0f, viewport.height - m_PopulationTxt->GetHeight() * 2 - 4.0f},Rectf{});
    //m_GenerationTxt->Draw(Point2f{10.0f, viewport.height - m_GenerationTxt->GetHeight() * 3 - 4.0f},Rectf{});
    DrawStatePicker(viewport);
    DrawPausedText(viewport);
}
void Pathfinding::DrawStatePicker(Rectf viewport) const
{
    using enum NodeState;
    using namespace PrettyColors;
    using namespace utils;

    float rectSize{40.0f};
    float drawOffset{rectSize+20.0f};
    Rectf statePickerRect{viewport.width - drawOffset,
        viewport.height - drawOffset,
        rectSize, rectSize};

    switch (m_StatePicked)
    {
    case empty:
        SetColor(GetColor(white));
        break;
    case bridge:
        SetColor(GetColor(brown));
        break;
    case water:
        SetColor(GetColor(blue));
        break;
    case rock:
        SetColor(GetColor(gray));
        break;
    case end:
        SetColor(GetColor(dRed));
        break;
    case start:
        SetColor(GetColor(dGreen));
        break;
    default:
        SetColor(GetColor(white));
        break;
    }
    FillRect(statePickerRect);
    SetColor(GetColor(white));
    DrawRect(statePickerRect,1.0f);

    DrawStatePickerText(statePickerRect,true);
}
void Pathfinding::DrawStatePickerText(Rectf statePickerPos,bool draw) const
{
    if (draw == false)
        return;
  
    Rectf drawDst{statePickerPos.left - m_StatePickerText->GetWidth() - 10.0f
        , statePickerPos.bottom + 5.0f
        , m_StatePickerText->GetWidth()
        , m_StatePickerText->GetHeight()};
    m_StatePickerText->Draw(drawDst);
}
void Pathfinding::DrawPausedText(Rectf viewport) const
{
    float edgeOffset{20.0f};
    Rectf drawDst{viewport.width - m_TimeState->GetWidth() - edgeOffset
    ,viewport.bottom + edgeOffset
    ,m_TimeState->GetWidth()
    ,m_TimeState->GetHeight()};
    m_TimeState->Draw(drawDst);
}

void Pathfinding::Update(float elapsedTime)
{

    using namespace std;
    m_Time->Update(elapsedTime);
    m_LBHeldTimer->Update(elapsedTime);
    UpdateTextures();
    if (m_Paused == true)
        return;

    if (OneStartAndEndPoint())
    {
        AStarAlgorithm();
    }

    m_FrameCounter += 1;
    if (m_FrameCounter > 5) // 10 == every 10 frames drawn
    {
        m_FrameCounter = 0;

    }

   

}
void Pathfinding::UpdateTextures()
{
    //m_PopulationTxt = std::make_unique<Texture>("Population: " + std::to_string(m_Population),"Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1});
    //m_GenerationTxt = std::make_unique<Texture>("Generation: " + std::to_string(m_Generation),"Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1});
    if (m_TimeStateChanged)
    {
        UpdatePausedText();
        m_TimeStateChanged = false;
    }
    if (m_StatePickedChanged)
    {
        UpdateStatePickerText();
        m_StatePickedChanged = false;
    }
}
void Pathfinding::UpdateStatePickerText()
{
    using namespace PrettyColors;
    using enum NodeState;
    int ptSize{24};
    Color4f color{GetColor(white)};
    const char* path {"Resources/Fonts/consola.ttf"};
    const char* text{"No state"};

    switch (m_StatePicked)
    {
    case empty:
        text = "Empty: ";
        break;
    case bridge:
        text = "Bridge: ";
        break;
    case water:
        text = "Water: ";
        break;
    case rock:
        text = "Rock: ";
        break;
    case end:
        text = "End: ";
        break;
    case start:
        text = "Start: ";
        break;
    default:
        break;
    }
    m_StatePickerText = std::make_unique<Texture>(text,path,ptSize,color);
}
void Pathfinding::UpdatePausedText()
{
    using namespace PrettyColors;
    using enum NodeState;
    int ptSize{24};
    Color4f color{GetColor(white)};
    const char* path{"Resources/Fonts/consola.ttf"};
    const char* text{"No state"};
    switch (m_Paused)
    {
    case true:
        text = "Paused";
        break;
    case false:
        text = "Going";
        break;
    }
    m_TimeState = std::make_unique<Texture>(text,path,ptSize,color);
}

void Pathfinding::AStarAlgorithm()
{
    using enum AStarState;
    int startNodeId{-1};
    int endNodeId{-1};
    // Set end and start nodes ids
    for (int i{0}; i < m_Nodes.size(); i++)
    {
        if (startNodeId == -1)
        {
            if (m_Nodes[i].state == NodeState::start)
            {
                startNodeId = i;
            }
        }
        if (endNodeId == -1)
        {
            if (m_Nodes[i].state == NodeState::end)
            {
                endNodeId = i;
            }
        }
        if (endNodeId != -1 && startNodeId != -1)
            break;
    }

    // Setup start node
    m_OpenList.push_back(startNodeId);
    m_Nodes[startNodeId].list = OpenList;
    m_Nodes[startNodeId].gCost = 0.0f;
    m_Nodes[startNodeId].hCost = (float)CalculateDistanceBetweenNodes(startNodeId,endNodeId);
    m_Nodes[startNodeId].fCost = m_Nodes[startNodeId].gCost + m_Nodes[startNodeId].hCost;

    // AStar
    while (m_EndFound == false && m_NoPathPossible == false)
    {
        float lowestFCost{m_DefaultNodeCosts};
        int lowestFCostOpenListId{-1};
        int currentNodeId{-1};

        for (int index{0}; index < m_OpenList.size(); index++)
        {
            // Find lowest fCost node
            if (m_Nodes[m_OpenList[index]].fCost < lowestFCost)
            {
                lowestFCost = m_Nodes[m_OpenList[index]].fCost;
                currentNodeId = m_OpenList[index];
                lowestFCostOpenListId = index;
            }
            // If fCosts are equal, find lower hCost node
            else if (m_Nodes[m_OpenList[index]].fCost == lowestFCost)
            {
                if (m_Nodes[m_OpenList[index]].hCost < m_Nodes[m_OpenList[lowestFCostOpenListId]].hCost)
                {
                    currentNodeId = m_OpenList[index];
                    lowestFCostOpenListId = index;
                }
            }
        }

        if (m_OpenList.size() == 0)
        {
            m_NoPathPossible = true;
            return;
        }
        m_OpenList.erase(m_OpenList.begin() + lowestFCostOpenListId);
        m_Nodes[currentNodeId].list = NoList;
        m_ClosedList.push_back(currentNodeId);
        m_Nodes[currentNodeId].list = ClosedList;

        // Check if end node found
        if (m_Nodes[currentNodeId].state == NodeState::end)
        {
            m_EndFound = true;
            break;
        }

        for (int i{0}; i < m_NeighborsOffsets.size(); i++)
        {
            // Won't overflow since you can't change edge nodes from rocks (so they can't be part of a path)
            int index{currentNodeId + m_NeighborsOffsets[i]};

            if (m_Nodes[index].state == NodeState::end)
            {
                m_Nodes[index].parentId = currentNodeId;
            }

            if (m_Nodes[index].state == NodeState::rock ||
                m_Nodes[index].state == NodeState::water ||
                m_Nodes[index].list == ClosedList)
            {
                continue;
            }

            int newPath{CalculateDistanceBetweenNodes(currentNodeId,index) + (int)m_Nodes[currentNodeId].gCost};
            if (m_Nodes[index].list != OpenList || m_Nodes[index].gCost > newPath) 
            // Probably useless check for open list (since if it's not in the openList it can be only in NoList (since above we check for closed)) FIX
            {
                m_Nodes[index].parentId = currentNodeId;
                m_Nodes[index].hCost = (float)CalculateDistanceBetweenNodes(index,endNodeId);
                m_Nodes[index].gCost = (float)CalculateDistanceBetweenNodes(m_Nodes[index].parentId,index) + m_Nodes[m_Nodes[index].parentId].gCost;
                m_Nodes[index].fCost = m_Nodes[index].gCost + m_Nodes[index].hCost;
            }
            if (m_Nodes[index].list != OpenList)
            {
                m_OpenList.push_back(index);
                m_Nodes[index].list = OpenList;
            }
        }


    }

    // Setup nodes that are part of path
    int index{endNodeId};
    while(m_PathSet == false && m_NoPathPossible == false)
    {
        index = m_Nodes[index].parentId;
        if (index == m_Nodes[startNodeId].id)
        {
            m_PathSet = true;
            return;
        }
        m_Nodes[index].partOfPath = true;
    }
}

int Pathfinding::CalculateDistanceBetweenNodes(int firstId,int secondId) const
{
    // One step is 10. Or 14 diagonally
    int straightStepCost{10};
    int diagonalStepCost{14}; // normalStep * 1.4

    int rowFirst = firstId / m_NodesPerRow;
    int colFirst = firstId % m_NodesPerRow;
    int rowSecond = secondId / m_NodesPerRow;
    int colSecond = secondId % m_NodesPerRow;

    int usingDiagonals{std::max(abs(rowFirst - rowSecond),abs(colFirst - colSecond))};
    int notUsingDiagonals{abs(rowFirst - rowSecond)+abs(colFirst - colSecond)};

    int amountOfDiagonalSteps{notUsingDiagonals - usingDiagonals};
    int amountOfStraightSteps{usingDiagonals - amountOfDiagonalSteps};

    int distance{amountOfDiagonalSteps* diagonalStepCost + amountOfStraightSteps* straightStepCost};

    return distance;
}
bool Pathfinding::OneStartAndEndPoint() const
{
    int startAmount{0};
    int endAmount{0};
    for (int i{0}; i < m_Nodes.size(); i++)
    {
        if (m_Nodes[i].state == NodeState::start)
        {
            startAmount += 1;
        }
        else if (m_Nodes[i].state == NodeState::end)
        {
            endAmount += 1;
        }
        if (startAmount > 1 || endAmount > 1)
        {
            return false;
        }
    }
    if (startAmount == 1 && endAmount == 1)
    {
        return true;
    }
    return false;
}

void Pathfinding::Start()
{
    m_TimeStateChanged = true;
    m_Paused = false;
}
void Pathfinding::Pause()
{
    m_TimeStateChanged = true;
    m_Paused = true;
}
void Pathfinding::FullRestart()
{
    for (int i{0}; i < m_Nodes.size(); i++)
    {
        m_Nodes[i].state = NodeState::empty;

    }
    RestartAStar();
}
void Pathfinding::RestartAStar()
{
    m_TimeStateChanged = true;
    m_Paused = true;
    for (int i{0}; i < m_Nodes.size(); i++)
    {
        m_Nodes[i].list = AStarState::NoList;
        m_Nodes[i].parentId = m_Nodes[i].id;
        m_Nodes[i].partOfPath = false;
    }
    m_OpenList.clear();
    m_ClosedList.clear();
    SetEdgeNodes();
    m_EndFound = false;
    m_PathSet = false;
    m_NoPathPossible = false;
}

void Pathfinding::CycleStateUp()
{
    using enum NodeState;
    m_StatePicked = static_cast<NodeState>(int(m_StatePicked)-1);
    if ((int)m_StatePicked < 0)
    {
        m_StatePicked = static_cast<NodeState>(int(TotalStates) - 1);
    }       
    m_StatePickedChanged = true;
}
void Pathfinding::CycleStateDown()
{
    using enum NodeState;
    m_StatePicked = static_cast<NodeState>(int(m_StatePicked) + 1);
    if (m_StatePicked >= TotalStates)
    {
        m_StatePicked = static_cast<NodeState>(0);
    }
    m_StatePickedChanged = true;
}
int Pathfinding::NodeOverlapping(Point2f target) const
{
    using namespace utils;
    for (int i{0}; i < m_Nodes.size(); i++)
    {
        int row = i / m_NodesPerRow;
        int col = i % m_NodesPerRow;

        float center = (float)m_NodesPerRow / 2.0f;
        float colOffset = center - col;
        float rowOffset = center - row;
        float yOffset{};
        float xOffset{};

        if (m_ScaleFactor == 1.0f)
        {
            xOffset = 0.0f;
            yOffset = 0.0f;
        }
        else if (m_ScaleFactor > 1.0f)
        {
            xOffset = -(colOffset * m_NodeSize * (m_ScaleFactor - 1.0f));
            yOffset = -(rowOffset * m_NodeSize * (m_ScaleFactor - 1.0f));
        }
        else
        {
            xOffset = (colOffset * m_NodeSize * (1.0f - m_ScaleFactor));
            yOffset = (rowOffset * m_NodeSize * (1.0f - m_ScaleFactor));
        }

        // Offset to avoid clicking multiple nodes at once
        float overlapOffset{0.1f};
        Rectf NodeRect{};
        NodeRect = Rectf{(m_GridPos.x + xOffset + col * m_NodeSize) + overlapOffset
            ,(m_GridPos.y + yOffset + row * m_NodeSize) + overlapOffset
            ,(m_NodeSize * m_ScaleFactor) - overlapOffset * 2
            ,(m_NodeSize * m_ScaleFactor) - overlapOffset * 2};

        if (IsPointInRect(target,NodeRect))
        {
            return m_Nodes[i].id;
        }
    }
    return -1;
}

void Pathfinding::OnMouseDown(const SDL_MouseButtonEvent& e)
{
    m_LastMousePos = OrientationManager::GetWorldLocation(Point2f{float(e.x),float(e.y)});
    if (m_Paused == false)
        return;

    using namespace utils; 
    float widthOffset{m_LineWidth / 2.0f};
    m_LBHeldTimer->Start();

    int nodeId{NodeOverlapping(m_LastMousePos)};
    if (nodeId != -1)
    {
        if (m_Nodes[nodeId].edge == true)
            return;
        m_Nodes[nodeId].state = m_StatePicked;
    }
    
}
void Pathfinding::OnMouseUp(const SDL_MouseButtonEvent& e)
{
    m_LastMousePos = OrientationManager::GetWorldLocation(Point2f{float(e.x),float(e.y)});

    m_LBHeldTimer->RestartAndPause();
    for (int i{0}; i < m_Nodes.size(); i++)
    {
        m_Nodes[i].drawnTo = false;
    }
}
void Pathfinding::OnMouseMotion(const SDL_MouseMotionEvent& e)
{
    m_LastMousePos = OrientationManager::GetWorldLocation(Point2f{float(e.x),float(e.y)});
    if(m_Paused == false)
        return;

    int nodeId{NodeOverlapping(m_LastMousePos)};
    if (m_LBHeldTimer->IsDone())
    {
        if (nodeId != -1)
        {
            if (m_Nodes[nodeId].edge == true)
                return;
            if (m_Nodes[nodeId].drawnTo == false)
            {
                m_Nodes[nodeId].state = m_StatePicked;
                m_Nodes[nodeId].drawnTo = true;
            }
        }
        
    }
}

Point2f Pathfinding::GetCenterOfGrid() const
{
    return Point2f{(m_NodesPerRow / 2) * (m_NodeSize)
    ,(m_NodesPerRow / 2) * (m_NodeSize)};
}
float Pathfinding::GetScale() const
{
    return m_ScaleFactor;
}