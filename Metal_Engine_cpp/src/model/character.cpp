#include "character.h"
#include "../backend/mtlm.h"

static const simd::float3 directionVectors[6] = {
    {  1.0f,  0.0f,  0.0f }, // +X
    {  0.0f,  1.0f,  0.0f }, // +Y
    {  0.0f,  0.0f,  1.0f }, // +Z
    { -1.0f,  0.0f,  0.0f }, // -X
    {  0.0f, -1.0f,  0.0f }, // -Y
    {  0.0f,  0.0f, -1.0f }  // -Z
};

int directionNumberFromVector(simd::float3 v) {
    if (v[0] ==  1.0f) return 0;
    if (v[1] ==  1.0f) return 1;
    if (v[2] ==  1.0f) return 2;
    if (v[0] == -1.0f) return 3;
    if (v[1] == -1.0f) return 4;
    if (v[2] == -1.0f) return 5;

    return -1; // invalid
}

void Character::setMesh(Mesh* mesh)
{
    this->mesh = mesh;
}

void Character::setPosition(simd::float3 pos)
{
    this->position = pos;
}

simd::float3 Character::getPosition()
{
    return position;
}

simd::float3 Character::getForwardVector()
{
    return directionVectors[forwardDirection];
}

simd::float3 Character::getUpVector()
{
    return directionVectors[upDirection];
}

void Character::setDirection(int forwardDirection, int upDirection)
{
    this->forwardDirection = forwardDirection;
    this->upDirection = upDirection;
    // update rotation matrix based on forward and up directions
    simd::float3 forwardVec, upVec;
    forwardVec = directionVectors[forwardDirection];
    upVec = directionVectors[upDirection];
    simd::float3 rightVec = simd::cross(upVec, forwardVec);
    rotation = mtlm::matrix_from_basis(rightVec, upVec, forwardVec);
}

void Character::counterclockwiseTurn()
{
    // Rotate forward and up directions counterclockwise around the Y axis
    simd::float3 forwardVec = directionVectors[forwardDirection];
    simd::float3 upVec = directionVectors[upDirection];
    simd::float3 newForwardVec = simd::cross(upVec, forwardVec);
    int newForward = directionNumberFromVector(newForwardVec);
    setDirection(newForward, upDirection);
}

void Character::draw(MTL::RenderCommandEncoder* encoder)
{
    if (mesh) {
        simd::float4x4 instanceData = mtlm::translation(position) * mtlm::translation({0.5f, 0.5f, 0.5f})* rotation * mtlm::translation({-0.5f, -0.5f, -0.5f}); // rotate around center of mesh
        encoder->setVertexBytes(&instanceData, sizeof(simd::float4x4), 2);
        mesh->draw(encoder);
    }
}

void Character::update(Battlefield* battlefield)
{
    if (isMoving) {
        simd::float3 forwardVec = directionVectors[forwardDirection];
        //check if the next position in the forward direction is occupied by a block in the battlefield
        simd::float3 nextPos = position + forwardVec * speed;
        simd::float3 forwardBound = {0.5f, 0.5f, 0.5f}; 
        forwardBound += position + forwardVec * 0.5f; // check the block in front of the character's bounding box
        int x = static_cast<int>(forwardBound[0]);
        int y = static_cast<int>(forwardBound[1]);
        int z = static_cast<int>(forwardBound[2]);
        if (x >= 0 && x < battlefield->getLengthX() && y >= 0 && y < battlefield->getLengthY() && z >= 0 && z < battlefield->getLengthZ()) {
            int* blockLayout = battlefield->getBlockLayout();
            if (blockLayout[x + y * battlefield->getLengthX() + z * battlefield->getLengthX() * battlefield->getLengthY()] == 0) {
                position = nextPos;
            }
            else {
                isMoving = false; // stop moving if we hit a block
                // set the up direction to the opposite of the forward direction to simulate a simple jump over the block, and the forward direction to the old up direction
                int curx = static_cast<int>(position[0]);
                int cury = static_cast<int>(position[1]);
                int curz = static_cast<int>(position[2]);
                position = {float(curx), float(cury), float(curz)}; // snap to current block position
                int newForwardDirection = upDirection;
                int newUpDirection = (forwardDirection + 3) % 6; // opposite direction
                setDirection(newForwardDirection, newUpDirection);
            }
        }
    }
}

void Character::setMoving()
{
    isMoving = true;
}
