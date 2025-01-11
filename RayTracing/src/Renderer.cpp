#include "Renderer.h"

#include "Walnut/Random.h"

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b* 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);
		
		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		//no resize
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
	

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = { (float)x/(float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight()};
			coord = coord * 2.0f - 1.0f; //-1 -> 1
			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

void Renderer::Render(glm::vec4 rgba, glm::vec3 lightDir)
{
    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
            coord = coord * 2.0f - 1.0f; //-1 -> 1
            glm::vec4 color = PerPixel(coord, rgba, lightDir);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
        }
    }
    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;
	//rayDirection = glm::normalize(rayDirection);
	
	// (bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2 + az^2 - r^2) = 0

	//a = ray origin
	//b = ray direction
	//r = radius
	//t = hit distance

	//rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z;
	float a = glm::dot(rayDirection, rayDirection);

	float b = 2.0f * glm::dot(rayOrigin, rayDirection);

	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	//Quad formula Discriminant
	// b^2 - 4ac

	// (-b +- sqrt(discriminant))/(2*a)

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return glm::vec4(0, 0, 0, 1);
	
	
	float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

	glm::vec3 sphereOrigin(0.0f);
	glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;

	//normalizing made the colors brighter because If the discriminant were positive, closestT would be calculated, and hitPoint and normal would be derived from it. For example, if coord were (0.0f, 0.0f):
	/*1.	rayDirection = (0.0f, 0.0f, -1.0f).
		2.	a = 1.0, b = -4.0, c = 3.75.
		3.	discriminant = 1.0.
		Then:
	•	closestT = (-b - sqrt(discriminant)) / (2.0f * a) = (4.0 - 1.0) / 2.0 = 1.5.
		•	hitPoint = rayOrigin + rayDirection * closestT = (0.0f, 0.0f, 2.0f) + (0.0f, 0.0f, -1.0f) * 1.5 = (0.0f, 0.0f, 0.5f).
		•	normal = glm::normalize(hitPoint - sphereOrigin) = glm::normalize((0.0f, 0.0f, 0.5f)) = (0.0f, 0.0f, 1.0f).*/
	glm::vec3 normal = glm::normalize(hitPoint - sphereOrigin);

	//this defined a light source coming from the top right of the screen, pointing in the (-1,-1,-1) direction
	glm::vec3 lightDir = glm::normalize(glm::vec3(-1));

	//glm::max is used because any angle >= 90degrees wont have light hitting it
	float d = glm::max(glm::dot(normal, -lightDir), 0.0f); //==cos(angle)
	


	glm::vec3 sphereColor(1,0,1);
	normal = normal * 0.5f + 0.5f;// this is the same as (normal + 1)/2. it brings range from -1->1 to 0->1
	
	//since d is the normal of the surface dotted with the -direction of the light, multiplying it to our color gives us
	//	realistic looking lighting based off the direction of the rays of light
	sphereColor *= d;
	return glm::vec4(sphereColor, 1.0f);

	
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord, glm::vec4 rgba)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;
	float a = glm::dot(rayDirection, rayDirection);

	float b = 2.0f * glm::dot(rayOrigin, rayDirection);

	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return glm::vec4(0, 0, 0, 1);


	float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

	glm::vec3 sphereOrigin(0.0f);
	glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;

	glm::vec3 normal = glm::normalize(hitPoint - sphereOrigin);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1));

	float d = glm::max(glm::dot(normal, -lightDir), 0.0f); 

	rgba *= d;
	return glm::vec4(rgba);


}

glm::vec4 Renderer::PerPixel(glm::vec2 coord, glm::vec4 rgba, glm::vec3 lightDir)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;
	float a = glm::dot(rayDirection, rayDirection);

	float b = 2.0f * glm::dot(rayOrigin, rayDirection);

	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return glm::vec4(0, 0, 0, 1);


	float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

	glm::vec3 sphereOrigin(0.0f);
	glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;

	glm::vec3 normal = glm::normalize(hitPoint - sphereOrigin);

	float d = glm::max(glm::dot(normal, -lightDir), 0.0f);

	rgba *= d;
	return glm::vec4(rgba);
}
