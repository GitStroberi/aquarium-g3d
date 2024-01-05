#include "Bubbles.h"

void Bubbles::generateParticles()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> coordDistX(coord_min.x, coord_max.x);
    std::uniform_real_distribution<float> coordDistY(coord_min.y, coord_max.y);
    std::uniform_real_distribution<float> coordDistZ(coord_min.z, coord_max.z);
    std::uniform_real_distribution<float> speedDist(speedLimits.first, speedLimits.second);
    std::uniform_real_distribution<float> radiusDist(radiusLimits.first, radiusLimits.second);
    std::uniform_real_distribution<float> sizeDist(sizeLimits.first, sizeLimits.second);

    for (int i = 0; i < numberOfParticles; ++i) {
        glm::vec3 randomCoord = { coordDistX(gen), coordDistY(gen), coordDistZ(gen) };
        coords.push_back(randomCoord);
        original_coords.push_back(randomCoord);
        speed.push_back(speedDist(gen));
        radius.push_back(radiusDist(gen));
        size.push_back(sizeDist(gen));
    }
}

void Bubbles::updateParticles(float currentFrame)
{
    for (int i = 0; i < numberOfParticles; i++)
    {
        coords[i].x = original_coords[i].x + sinf(currentFrame * 2 * speed[i]) * radius[i];
        coords[i].z = original_coords[i].z + cosf(currentFrame * 2 * speed[i]) * radius[i];
        coords[i].y += 0.002 * speed[i];
        if (coords[i].y >= coord_max.y)
            coords[i].y = coord_min.y;

        coords[i].x = std::max(std::min(coords[i].x, coord_max.x), coord_min.x);
        coords[i].z = std::max(std::min(coords[i].z, coord_max.z), coord_min.z);
    }
}

std::vector<BubbleParticle> Bubbles::getParticles() const
{
    std::vector<BubbleParticle> particles;
    for (int i = 0; i < numberOfParticles; ++i) {
        particles.emplace_back(coords[i], diffuseTextureId, meshVAO, std::make_tuple(coords[i], coords[i], coords[i], coords[i]), size[i]);
    }

    return particles;
}

double BubbleParticle::getDistanceToCamera(glm::vec3 cameraPos) const
{
    return glm::distance(position, cameraPos);
}

void BubbleParticle::renderMesh() const
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

glm::vec3 BubbleParticle::getPosition() const
{
    return position;
}

unsigned BubbleParticle::getDiffuseTextureId() const
{
    return diffuseTextureId;
}

unsigned BubbleParticle::getSize() const
{
    return size;
}