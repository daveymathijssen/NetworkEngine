#include "api.h"
#include "PanelComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/tien/components/DynamicSkyBox.h>


Api scene_panel_clear("scene/panel/clear", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, json &data)
{
	if (data.find("id") == data.end())
	{
		sendError(tunnel, "scene/panel/clear", "id not found");
		return;
	}
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	if (!node)
	{
		sendError(tunnel, "scene/panel/clear", "node not found");
		return;
	}
	PanelComponent* panel = node->getComponent<PanelComponent>();
	if (!panel)
	{
		sendError(tunnel, "scene/panel/clear", "panel component not found");
		return;
	}
	panel->clear();
	sendOk(tunnel, "scene/panel/clear");
});


Api scene_panel_swap("scene/panel/swap", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, json &data)
{
	if (data.find("id") == data.end())
	{
		sendError(tunnel, "scene/panel/swap", "id not found");
		return;
	}
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	if (!node)
	{
		sendError(tunnel, "scene/panel/swap", "node not found");
		return;
	}
	PanelComponent* panel = node->getComponent<PanelComponent>();
	if (!panel)
	{
		sendError(tunnel, "scene/panel/swap", "panel component not found");
		return;
	}
	panel->swap();
	sendOk(tunnel, "scene/panel/swap");
});

Api scene_panel_drawlines("scene/panel/drawlines", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, json &data)
{
	if (data.find("id") == data.end())
	{
		sendError(tunnel, "scene/panel/drawlines", "id not found");
		return;
	}
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	if (!node)
	{
		sendError(tunnel, "scene/panel/drawlines", "node not found");
		return;
	}
	PanelComponent* panel = node->getComponent<PanelComponent>();
	if (!panel)
	{
		sendError(tunnel, "scene/panel/drawlines", "panel component not found");
		return;
	}



	std::vector<vrlib::gl::VertexP3C4> verts;
	for (size_t i = 0; i < data["lines"].size(); i++)
	{
		glm::vec4 color(data["lines"][i][4],
						data["lines"][i][5],
						data["lines"][i][6],
						data["lines"][i][7]);

		verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(data["lines"][i][0], data["lines"][i][1], 0), color));
		verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(data["lines"][i][2], data["lines"][i][3], 0), color));
	}
	
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, panel->backFbo->getWidth(), panel->backFbo->getHeight());
	panel->backFbo->bind();

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	engine->debugShader->use();
	engine->debugShader->setUniform(NetworkEngine::DebugUniform::projectionMatrix, glm::ortho(0.0f, (float)panel->backFbo->getWidth(), (float)panel->backFbo->getHeight(), 0.0f));
	engine->debugShader->setUniform(NetworkEngine::DebugUniform::modelViewMatrix, glm::mat4());

	vrlib::gl::setAttributes<vrlib::gl::VertexP3C4>(&verts[0]);
	glLineWidth(data["width"]);
	glDrawArrays(GL_LINES, 0, verts.size());
	glEnable(GL_DEPTH_TEST);



	panel->backFbo->unbind();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	json packet;
	packet["id"] = "scene/panel/drawlines";
	packet["data"]["status"] = "ok";
	tunnel->send(packet);
});


Api scene_panel_drawtext("scene/panel/drawtext", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, json &data)
{
	if (data.find("id") == data.end())
	{
		sendError(tunnel, "scene/panel/drawtext", "id not found");
		return;
	}
	if (data.find("position") == data.end())
	{
		sendError(tunnel, "scene/panel/drawtext", "position field not set");
		return;
	}
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	if (!node)
	{
		sendError(tunnel, "scene/panel/drawtext", "node not found");
		return;
	}
	PanelComponent* panel = node->getComponent<PanelComponent>();
	if (!panel)
	{
		sendError(tunnel, "scene/panel/drawtext", "panel component not found");
		return;
	}


	std::string font = "segoeui";
	float size = 32;
	glm::vec4 color = glm::vec4(0, 0, 0, 1);

	if (data.find("font") != data.end())
		font = data["font"];
	if (data.find("size") != data.end())
		size = data["size"];
	if (data.find("color") != data.end())
		color = glm::vec4(data["color"][0], data["color"][1], data["color"][2], data["color"][3]);

	if (!panel->drawText(glm::vec2(data["position"][0], data["position"][1]), font, data["text"], color, size))
		sendError(tunnel, "scene/panel/drawtext", "Error loading font");
	else
		sendOk(tunnel, "scene/panel/drawtext");
});


Api scene_panel_setclearcolor("scene/panel/setclearcolor", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, json &data)
{
	if (data.find("id") == data.end())
	{
		sendError(tunnel, "scene/panel/setclearcolor", "id not found");
		return;
	}
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	if (!node)
	{
		sendError(tunnel, "scene/panel/setclearcolor", "node not found");
		return;
	}
	PanelComponent* panel = node->getComponent<PanelComponent>();
	if (!panel)
	{
		sendError(tunnel, "scene/panel/setclearcolor", "panel component not found");
		return;
	}

	for (int i = 0; i < 4; i++)
		panel->clearColor[i] = data["color"][i];
	sendOk(tunnel, "scene/panel/setclearcolor");

});



Api scene_panel_image("scene/panel/image", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, json &data)
{
	if (data.find("id") == data.end())
	{
		sendError(tunnel, "scene/panel/image", "id not found");
		return;
	}
	if (data.find("image") == data.end())
	{
		sendError(tunnel, "scene/panel/image", "image field not set");
		return;
	}
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	if (!node)
	{
		sendError(tunnel, "scene/panel/image", "node not found");
		return;
	}
	PanelComponent* panel = node->getComponent<PanelComponent>();
	if (!panel)
	{
		sendError(tunnel, "scene/panel/image", "panel component not found");
		return;
	}

	glm::vec2 position(0, 0);
	glm::vec2 size(panel->backFbo->getWidth(), panel->backFbo->getHeight());

	if (data.find("position") == data.end())
		position = glm::vec2(data["position"][0], data["position"][1]);
	if (data.find("size") == data.end())
		position = glm::vec2(data["size"][0], data["size"][1]);

	if (!panel->drawImage(data["image"], position, size))
		sendError(tunnel, "scene/panel/drawtext", "Error drawing image");
	else
		sendOk(tunnel, "scene/panel/drawtext");
});