#include "ShiftD3D11Frame.h"
#include "ShiftWindow.h"
#include "./ui_shiftwindow.h"

#include "UserInterface/Editor_Window.h"
#include "UserInterface/Editor_UI.h"

#include "Game/CAMERA.H"
#include "Game/PSX/COLLIDES.H"

#include <QTime>
#include <QPainter>

#include "ShiftRightPane.h"

#include <thread>

extern struct Camera theCamera;

extern struct _Rotation overrideEditorRotation;
extern struct _Position overrideEditorPosition;

extern char* GAMELOOP_GetBaseAreaName();

extern HWND g_overrideHWND;
extern int g_overrideWidth;
extern int g_overrideHeight;

void Shift::D3D11Frame::initialiseHWND(HWND windowHandle, int width, int height)
{
    g_overrideHWND = windowHandle;
    g_overrideWidth = width;
    g_overrideHeight = height;
}

void Shift::D3D11Frame::render()
{
    static char lastAreaName[32];

    if (strcmp(lastAreaName, GAMELOOP_GetBaseAreaName()))
    {
        strcpy(lastAreaName, GAMELOOP_GetBaseAreaName());
        g_ShiftWindow->getPanes()->m_centerPane->getTabWidget()->setTabText(0, lastAreaName);
    }
}

void Shift::D3D11Frame::paintEvent(QPaintEvent* event)
{
    
}

QPaintEngine* Shift::D3D11Frame::paintEngine()
{
    return NULL;
}

void Shift::D3D11Frame::resizeEvent(QResizeEvent* event)
{
    int oldW = event->size().width();
    int oldH = event->size().height();

    g_overrideWidth = oldW;
    g_overrideHeight = oldH;
}

void Shift::D3D11Frame::keyPressEvent(QKeyEvent* event)
{
    int speed = 8;

    if (event->modifiers() & Qt::ControlModifier)  speed /= 2;
    if (event->modifiers() & Qt::ShiftModifier) speed *= 2;

    if (event->key() == Qt::Key_W)
    {
        short theta = overrideEditorRotation.x;
        short phi = overrideEditorRotation.z;

        int xMove = speed * rsin(phi) * rcos(theta);
        int yMove = speed * rcos(phi) * rcos(theta);
        int zMove = speed * rsin(theta);

        overrideEditorPosition.x -= xMove >> 20;
        overrideEditorPosition.y += yMove >> 20;
        overrideEditorPosition.z += zMove >> 8;
    }
    
    if (event->key() == Qt::Key_S)
    {
        short theta = overrideEditorRotation.x;
        short phi = overrideEditorRotation.z;

        int xMove = speed * rsin(phi) * rcos(theta);
        int yMove = speed * rcos(phi) * rcos(theta);
        int zMove = speed * rsin(theta);

        overrideEditorPosition.x += xMove >> 20;
        overrideEditorPosition.y -= yMove >> 20;
        overrideEditorPosition.z -= zMove >> 8;
    }

    if (event->key() == Qt::Key_A)
    {
        short theta = overrideEditorRotation.x;
        short phi = overrideEditorRotation.z;

        int xMove = speed * rcos(phi) * rcos(theta);
        int yMove = speed * rsin(phi) * rcos(theta);
        int zMove = speed * rsin(theta);

        overrideEditorPosition.x -= xMove >> 20;
        overrideEditorPosition.y -= yMove >> 20;
        overrideEditorPosition.z += zMove >> 8;
    }

    if (event->key() == Qt::Key_D)
    {
        short theta = overrideEditorRotation.x;
        short phi = overrideEditorRotation.z;

        int xMove = speed * rcos(phi) * rcos(theta);
        int yMove = speed * rsin(phi) * rcos(theta);
        int zMove = speed * rsin(theta);

        overrideEditorPosition.x += xMove >> 20;
        overrideEditorPosition.y += yMove >> 20;
        overrideEditorPosition.z -= zMove >> 8;
    }
}

void Shift::D3D11Frame::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        ShowCursor(false);
        m_previousMousePosition = event->pos();
        m_dragging = true;
    }
}

void Shift::D3D11Frame::pickRayVector(float mouseX, float mouseY, float ClientWidth, float ClientHeight, DirectX::XMVECTOR& rayPos, DirectX::XMVECTOR& rayDir)
{
#if 0
    //Normalized device coordinates
    DirectX::XMVECTOR pickRayViewSpace = DirectX::XMVectorSet(
        mouseX = (((2.0f * mouseX) / ClientWidth) - 1) / Engine::g_camera.m_projectionMatrix.r[0].m128_f32[0],
        mouseY = -(((2.0f * mouseY) / ClientHeight) - 1) / Engine::g_camera.m_projectionMatrix.r[1].m128_f32[1],
        1.0f,
        0.0f);

    DirectX::XMVECTOR pickRayViewSpacePos = DirectX::XMVectorZero();
    DirectX::XMVECTOR pickRayViewSpaceDir = pickRayViewSpace;
    DirectX::XMMATRIX pickRayWorldSpace = DirectX::XMMatrixInverse(nullptr, Engine::g_camera.m_viewMatrix);

    pickRayViewSpacePos = DirectX::XMVector3TransformCoord(pickRayViewSpacePos, pickRayWorldSpace);
    pickRayViewSpaceDir = DirectX::XMVector3TransformNormal(pickRayViewSpaceDir, pickRayWorldSpace);

    rayPos = pickRayViewSpacePos;
    rayDir = pickRayViewSpaceDir;
#endif
}

#if 0
bool Shift::D3D11Frame::rayBoxIntersect(DirectX::XMVECTOR& rpos, DirectX::XMVECTOR& rdir, Engine::Vector3& boxMin, Engine::Vector3& boxMax, DirectX::XMMATRIX& transform)
{
    Engine::Vector3 dirfrac;
    // r.dir is unit direction vector of ray
    dirfrac.x = 1.0f / rdir.m128_f32[0];
    dirfrac.y = 1.0f / rdir.m128_f32[1];
    dirfrac.z = 1.0f / rdir.m128_f32[2];

    DirectX::XMVECTOR vMin = DirectX::XMVectorSet(boxMin.x, boxMin.y, boxMin.z, 0.0f);
    DirectX::XMVECTOR vMax = DirectX::XMVectorSet(boxMax.x, boxMax.y, boxMax.z, 0.0f);
    DirectX::XMVECTOR vNewMin = DirectX::XMVectorZero();
    DirectX::XMVECTOR vNewMax = DirectX::XMVectorZero();

    vNewMin = DirectX::XMVector3TransformCoord(vMin, transform);
    vNewMax = DirectX::XMVector3TransformCoord(vMax, transform);

    vMin = DirectX::XMVectorMin(vNewMin, vNewMax);
    vMax = DirectX::XMVectorMax(vNewMin, vNewMax);

    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    float t1 = (vMin.m128_f32[0] - rpos.m128_f32[0]) * dirfrac.x;
    float t2 = (vMax.m128_f32[0] - rpos.m128_f32[0]) * dirfrac.x;
    float t3 = (vMin.m128_f32[1] - rpos.m128_f32[1]) * dirfrac.y;
    float t4 = (vMax.m128_f32[1] - rpos.m128_f32[1]) * dirfrac.y;
    float t5 = (vMin.m128_f32[2] - rpos.m128_f32[2]) * dirfrac.z;
    float t6 = (vMax.m128_f32[2] - rpos.m128_f32[2]) * dirfrac.z;

    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0)
    {
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
        return false;
    }

    return true;
}
#endif

void Shift::D3D11Frame::CheckIfRayIntersectedWithAnySceneObject(DirectX::XMVECTOR& rayPosition, DirectX::XMVECTOR& rayDirection)
{
    bool bSelected = false;

    //Engine::Vector3* sceneTranslationOffset = g_engine.m_scene.getSceneOffset();
    //DirectX::XMMATRIX sceneMatrix = DirectX::XMMatrixTranslation(-sceneTranslationOffset->x, -sceneTranslationOffset->y, -sceneTranslationOffset->z);

    //Deselect
    //if (g_engine.m_scene.m_selectedRenderable != nullptr)
    {
        //g_engine.m_scene.m_selectedRenderable->m_bDrawAABB[g_engine.m_scene.m_selectedRenderableInstance] = false;
        //g_engine.m_scene.m_selectedRenderableInstance = -1;
        //g_engine.m_scene.m_selectedRenderable = nullptr;
    }

    float closestDist = 100000000.0f;

#if 0

    for (size_t i = 0; i < g_engine.m_scene.m_renderables.size(); i++)
    {
        Engine::Resource::RenderableMesh* renderableMesh = &g_engine.m_scene.m_renderables[i];

        for (unsigned int j = 0; j < renderableMesh->m_instanceCount; j++)
        {
            DirectX::XMMATRIX transform;
            memcpy(&transform, &renderableMesh->m_transform[j], sizeof(DirectX::XMMATRIX));
            transform *= sceneMatrix;

            if (Engine::g_camera.cullAABB(renderableMesh->m_boxMin, renderableMesh->m_boxMax, transform))
            {
                continue;
            }

            float objectDistanceToCamera = Engine::GetDistanceToCamera(transform.r[3]);

            //If aabb intersected with ray and object distance to camera is < closest distance so far.
            if (rayBoxIntersect(rayPosition, rayDirection, renderableMesh->m_boxMin, renderableMesh->m_boxMax, transform) && objectDistanceToCamera < closestDist)
            {
                g_engine.m_scene.m_selectedRenderable = renderableMesh;
                g_engine.m_scene.m_selectedRenderableInstance = j;
                closestDist = objectDistanceToCamera;
            }
            else
            {
                renderableMesh->m_bDrawAABB[j] = false;
            }
        }
    }

    if (g_engine.m_scene.m_selectedRenderable != nullptr)
    {
        g_engine.m_scene.m_selectedRenderable->m_bDrawAABB[g_engine.m_scene.m_selectedRenderableInstance] = true;
        m_panes->m_rightPane->populateRenderableProperties(g_engine.m_scene.m_selectedRenderable, g_engine.m_scene.m_selectedRenderableInstance);
    }
    else
    {
        m_panes->m_rightPane->populateEmpty();
    }
#endif

    return;
}

QSize Shift::D3D11Frame::getRatio(QSize currentResolution)
{
    int w = currentResolution.width();
    int h = currentResolution.height();

    if ((w / h) < (16 / 9))
    {
        w = (w / 16) * 16;
        h = (w / 16) * 9;
    }
    else
    {
        h = (h / 9) * 9;
        w = (h / 9) * 16;
    }

    return QSize(w, h);
}

void Shift::D3D11Frame::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        ShowCursor(true);
        m_dragging = false;
    }
    else if (event->button() == Qt::LeftButton)
    {
        static DirectX::XMVECTOR rayPosition, rayDirection;
        pickRayVector(event->localPos().x(), event->localPos().y(), windowWidthf, windowHeightf, rayPosition, rayDirection);
        CheckIfRayIntersectedWithAnySceneObject(rayPosition, rayDirection);
    }
}

void Shift::D3D11Frame::mouseMoveEvent(QMouseEvent* event)
{
    QPoint delta = event->pos() - m_previousMousePosition;

    if (m_dragging)
    {
#define CAM_MOUSE_SENSITIVITY 2

        int cameraRotZ = delta.x() * CAM_MOUSE_SENSITIVITY % 4096;
        int cameraRotX = delta.y() * CAM_MOUSE_SENSITIVITY % 4096;
        
        overrideEditorRotation.z += cameraRotZ;
        overrideEditorRotation.x += cameraRotX;

        m_previousMousePosition = event->pos();
        return;
    }
}

void Shift::D3D11Frame::renderWidget()
{
    
}