#include "../lib/AGL/agl.hpp"

int main()
{
	agl::RenderWindow window;
	window.setup({500, 500}, "winfloat");
	window.setClearColor(agl::Color::Black);
	window.setFPS(30);

	agl::Event event;
	event.setWindow(window);

	agl::ShaderBuilder sbFrag;
	sbFrag.setDefaultFrag();
	std::string fragSrc = sbFrag.getSrc();

	agl::ShaderBuilder sbVert;
	sbVert.setDefaultVert();
	std::string vertSrc = sbVert.getSrc();

	agl::Shader shader;
	shader.compileSrc(vertSrc, fragSrc);

	agl::ShaderBuilder graphVertBuilder;
	graphVertBuilder.addLayout(0, agl::vec3, "position");
	graphVertBuilder.addLayout(1, agl::vec2, "vertexUV");

	graphVertBuilder.addUniform(agl::mat4, "transform");
	graphVertBuilder.addUniform(agl::mat4, "mvp");
	graphVertBuilder.addUniform(agl::vec3, "shapeColor");
	graphVertBuilder.addUniform(agl::mat4, "textureTransform");

	graphVertBuilder.addOut(agl::vec2, "UVcoord");
	graphVertBuilder.addOut(agl::vec4, "fragColor");

	graphVertBuilder.setMain({
		agl::val("UVcoord")		  = agl::val("vec2((textureTransform") * agl::val("vec4(vertexUV, 1, 1)).xy)"), //
		agl::val("fragColor")	  = agl::val("vec4(shapeColor, 1)"),											//
		agl::val("gl_Position")	  = agl::val("transform") * agl::val("vec4(position, 1)"),						//
		agl::val("float x")		  = agl::val("gl_Position.x"),													//
		agl::val("float y")		  = agl::val("gl_Position.y"),													//
		agl::val("float z")		  = agl::val("gl_Position.z"),													//
		agl::val("y") = agl::val("pow(x, 2.0) - z"),
		agl::val("gl_Position.x") = agl::val("x"),																//
		agl::val("gl_Position.y") = agl::val("y"),																//
		agl::val("gl_Position.z") = agl::val("z"),																//
		agl::val("gl_Position")	  = agl::val("mvp") * agl::val("gl_Position")									//
	});

	std::string graphVert = graphVertBuilder.getSrc();

	agl::Shader graphVertSrc;
	graphVertSrc.compileSrc(graphVert, fragSrc);

	window.getShaderUniforms(shader);
	shader.use();

	agl::Camera camera;
	camera.setView({3, 3, 3}, {0, 0, 0}, {0, 1, 0});
	camera.setPerspectiveProjection(45,			 // set FOV to 45
									500. / 500., // aspect ration is 1
									0.1,		 // if closer than 0.1 than do not render
									1000		 // if further than 1000 than do not render
	);

	window.updateMvp(camera);

	agl::Texture texture;
	texture.loadFromFile("dtrj.png");
	// texture.setBlank();

	agl::Texture blank;
	blank.setBlank();

	agl::Rectangle rectangle;
	rectangle.setColor({255, 0, 0});
	rectangle.setTexture(&texture);
	rectangle.setPosition({0, 0, 0});
	rectangle.setSize(agl::Vec<float, 3>{1, 1, 0});
	rectangle.setRotation(agl::Vec<float, 3>{1, 0, 0});
	rectangle.setOffset({0, 0, 0});

	agl::Shape plane([&](agl::Shape &shape) {
		constexpr int detail = 30;
		constexpr int verts	 = 6 * detail * detail;

		float vertexBufferData[verts * 3];
		float UVBufferData[verts * 2];

		float size = 2. / detail;

		int triangle = 0;

		for (int y = 0; y < detail; y++)
		{
			float offsety = ((y / (float)detail) * 2) - 1;

			for (int x = 0; x < detail; x++)
			{
				float offsetx = ((x / (float)detail) * 2) - 1;

				vertexBufferData[(triangle * 3) + 0] = offsetx;
				vertexBufferData[(triangle * 3) + 1] = offsety;
				vertexBufferData[(triangle * 3) + 2] = 0;
				triangle++;

				vertexBufferData[(triangle * 3) + 0] = offsetx + size;
				vertexBufferData[(triangle * 3) + 1] = offsety;
				vertexBufferData[(triangle * 3) + 2] = 0;
				triangle++;

				vertexBufferData[(triangle * 3) + 0] = offsetx;
				vertexBufferData[(triangle * 3) + 1] = offsety + size;
				vertexBufferData[(triangle * 3) + 2] = 0;
				triangle++;

				vertexBufferData[(triangle * 3) + 0] = offsetx + size;
				vertexBufferData[(triangle * 3) + 1] = offsety + size;
				vertexBufferData[(triangle * 3) + 2] = 0;
				triangle++;

				vertexBufferData[(triangle * 3) + 0] = offsetx + size;
				vertexBufferData[(triangle * 3) + 1] = offsety;
				vertexBufferData[(triangle * 3) + 2] = 0;
				triangle++;

				vertexBufferData[(triangle * 3) + 0] = offsetx;
				vertexBufferData[(triangle * 3) + 1] = offsety + size;
				vertexBufferData[(triangle * 3) + 2] = 0;
				triangle++;
			}
		}

		for (int i = 0; i < verts; i++)
		{
			UVBufferData[(i * 2) + 0] = (vertexBufferData[(i * 3) + 0] + 1) / 2;
			UVBufferData[(i * 2) + 1] = (vertexBufferData[(i * 3) + 1] + 1) / 2;
		}

		shape.genBuffers();
		shape.setMode(GL_TRIANGLES);
		shape.setBufferData(vertexBufferData, UVBufferData, verts);
	});

	plane.setTexture(&texture);
	plane.setColor(agl::Color::White);
	plane.setPosition({0, 0, 0});
	plane.setSize({1, 1, 0});
	plane.setRotation({90, 0, 0});

	agl::Shape line([](agl::Shape &shape) {
		float vertexBufferData[6];
		float UVBufferData[4];

		vertexBufferData[0] = 0;
		vertexBufferData[1] = 0;
		vertexBufferData[2] = 0;
		vertexBufferData[3] = 1;
		vertexBufferData[4] = 1;
		vertexBufferData[5] = 0;

		UVBufferData[0] = vertexBufferData[0];
		UVBufferData[1] = vertexBufferData[1];
		UVBufferData[2] = vertexBufferData[3];
		UVBufferData[3] = vertexBufferData[4];

		shape.genBuffers();
		shape.setMode(GL_LINES);
		shape.setBufferData(vertexBufferData, UVBufferData, 2);
	});

	line.setTexture(&blank);
	line.setColor(agl::Color::White);

	while (!event.windowClose())
	{
		event.poll();

		window.clear();

		window.getShaderUniforms(graphVertSrc);
		graphVertSrc.use();
		window.updateMvp(camera);

		window.drawShape(plane);

		window.getShaderUniforms(shader);
		shader.use();
		window.updateMvp(camera);

		line.setOffset({0, 0, 0});
		line.setPosition({0, -20, 0});
		line.setSize({0, 40, 0});
		line.setRotation({0, 0, 0});
		window.drawShape(line);

		line.setPosition({0, 0, 0});

		line.setSize({40, 0, 0});
		line.setOffset({-20, 0, 0});
		window.drawShape(line);

		line.setRotation({0, 90, 0});
		window.drawShape(line);

		window.display();
	}

	texture.deleteTexture();
	shader.deleteProgram();
	window.close();

	return 0;
}
