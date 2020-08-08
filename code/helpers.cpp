#include "headers/helpers.h"

// ========================================

/** Loads external 3D model (.OBJ and .MTL files + textures). */
void loadModel(const string &filename, GLuint program, vector <Mesh*> &model)
{
	Importer importer; // get loader from Assimp library
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1); // normalize model
	const aiScene* scn = importer.ReadFile // load model
	(
		filename.c_str(),
		0 | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices
	);

	if (scn == NULL) // check for loading errors
		cerr << "ASSIMP ERROR: " << importer.GetErrorString() << endl;

	// ****************************************

	// load multiple meshes
	for (size_t i = 0; i < scn->mNumMeshes; i++)
	{
		const aiMesh* mesh = scn->mMeshes[i]; // get mesh

		Mesh *part = new Mesh();
		part->setNumTriangles(mesh->mNumFaces);

		// create vbo
		glGenBuffers(1, part->getAddressVbo());
		glBindBuffer(GL_ARRAY_BUFFER, part->getVbo());
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // store data
		glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices); // vertices
		glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals); // normals

		// texture coords
		float* textureCoords = new float[2 * mesh->mNumVertices];
		float* currentTextureCoord = textureCoords;

		aiVector3D vect;
		if (mesh->HasTextureCoords(0)) // copy texture coords
			for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) // use 2D textures and ignore third coord
			{
				vect = (mesh->mTextureCoords[0])[idx];
				*currentTextureCoord++ = vect.x;
				*currentTextureCoord++ = vect.y;
			} // for

		glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords); // coords

		// ****************************************

		// store triangle faces
		unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
		{
			indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
			indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
			indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
		} // for

		// create ebo
		glGenBuffers(1, part->getAddressEbo());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part->getEbo());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

		delete[] indices;

		// ****************************************

		// store material
		const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
		aiColor4D color;
		aiString name;
		aiReturn retValue = AI_SUCCESS;

		mat->Get(AI_MATKEY_NAME, name);

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		part->setDiffuse(vec3(color.r, color.g, color.b));

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		part->setAmbient(vec3(color.r, color.g, color.b));

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		part->setSpecular(vec3(color.r, color.g, color.b));

		ai_real shininess, strength;
		unsigned int max;

		max = 1;
		if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
			shininess = 1.0f;

		max = 1;
		if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
			strength = 1.0f;

		part->setShininess(shininess * strength);

		// ****************************************

		// load texture image
		part->setTexture(0);
		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path; // texture filename
			aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			string textureName = path.data;

			size_t found = filename.find_last_of("/\\");
			if (found != string::npos) // not found
				textureName.insert(0, filename.substr(0, found + 1));

			part->setTexture(createTexture(textureName));
			cout << "loading texture: " << textureName << endl;
		} // if

		// ****************************************

		// create vao
		glGenVertexArrays(1, part->getAddressVao());
		glBindVertexArray(part->getVao());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part->getEbo());
		glBindBuffer(GL_ARRAY_BUFFER, part->getVbo());

		// connect everything to vertex shader
		GLint posLoc = glGetAttribLocation(program, VERT_POS_VAR);
		glEnableVertexAttribArray(posLoc);
		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		GLint norLoc = glGetAttribLocation(program, VERT_NOR_VAR);
		glEnableVertexAttribArray(norLoc);
		glVertexAttribPointer(norLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

		GLint texCooLoc = glGetAttribLocation(program, TEX_COO_VAR);
		glEnableVertexAttribArray(texCooLoc);
		glVertexAttribPointer(texCooLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));

		glBindVertexArray(0);

		model.push_back(part); // insert mesh into the model
	} // for
} // CREATE MODEL

// ========================================

/** Checks collision with hangar. */
bool checkComplexCollision(vec3 myPos, vec3 myBoundBox, vec3 objPos, vec3 objBoundBox, vec3 objInBox)
{
	const float x = abs(myPos.x - objPos.x);
	const float y = abs(myPos.y);
	const float z = abs(myPos.z - objPos.z);

	if (((x - myBoundBox.x / 2.0f) <= objBoundBox.x) &&
		  ((y - myBoundBox.y / 2.0f) <= objBoundBox.y) &&
		  ((z - myBoundBox.z / 2.0f) <= objBoundBox.z)) // collision
	{
		if (((x - myBoundBox.x) <= objInBox.x) &&
			  ((y + myBoundBox.y / 3.0f) <= objInBox.y) &&
			  ((z + myBoundBox.z / 3.0f) <= objInBox.z)) // no collision if the player is inside the object
			return false;

		return true;
	} // if

	return false;
} // CHECK COMPLEX COLLISION

// ========================================

/** Checks collision with any other object except hangar. */
bool checkTrivialCollision(vec3 myPos, vec3 myBoundBox, vec3 objPos, vec3 objBoundBox)
{
	const float x = abs(myPos.x - objPos.x);
	const float y = abs(myPos.y - objPos.y);
	const float z = abs(myPos.z - objPos.z);

	if (((x - myBoundBox.x / 2.0f) <= objBoundBox.x) &&
		  ((y - myBoundBox.y / 2.0f) <= objBoundBox.y) &&
		  ((z - myBoundBox.z / 2.0f) <= objBoundBox.z)) // collision
	{
		if (cam->getPlane() && ((*cam->getPlane())->getCurrSpeed() >= 0.2f))
		{
			float time = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME);
			if (jetPlane && (objPos == jetPlane->getPos()))
			{
				explosions.push_back(new Explosion(objPos, vec3(0.0f), vec3(2.0f), vec3(0.0f), vec3(0.0f), 0.0f, time));
				deleteComponent(&jetPlane);
			} // if
			else if (fighterPlane && (objPos == fighterPlane->getPos()))
			{
				explosions.push_back(new Explosion(objPos, vec3(0.0f), vec3(2.0f), vec3(0.0f), vec3(0.0f), 0.0f, time));
				deleteComponent(&fighterPlane);
			} // else if
			else if (retroPlane && (objPos == retroPlane->getPos()))
			{
				explosions.push_back(new Explosion(objPos, vec3(0.0f), vec3(2.0f), vec3(0.0f), vec3(0.0f), 0.0f, time));
				deleteComponent(&retroPlane);
			} // else if
			else if (helicopter && (objPos == helicopter->getPos()))
			{
				explosions.push_back(new Explosion(objPos, vec3(0.0f), vec3(2.0f), vec3(0.0f), vec3(0.0f), 0.0f, time));
				deleteComponent(&helicopter);
			} // else if
		} // if

		return true;
	} // if

	return false;
} // CHECK TRIVIAL COLLISION

// ========================================

/** Checks collisions with all objects in the world. */
bool checkCollisions(vec3 myPos, vec3 myBoundBox)
{
	// check all bounding boxes
	if (checkTrivialCollision(myPos, myBoundBox, tower->getPos(), tower->getBoundBox()) || // tower
		  checkTrivialCollision(myPos, myBoundBox, antenna->getPos(), antenna->getBoundBox()) || // antenna
		  checkTrivialCollision(myPos, myBoundBox, stones[0]->getPos(), stones[0]->getBoundBox()) || // front stone
		  checkTrivialCollision(myPos, myBoundBox, stones[1]->getPos(), stones[1]->getBoundBox()) || // back stone
		  checkComplexCollision(myPos, myBoundBox, hangars[0]->getPos(), hangars[0]->getBoundBox(), vec3(6.0f, 3.0f, 3.0f)) || // front hangar
		  checkComplexCollision(myPos, myBoundBox, hangars[1]->getPos(), hangars[1]->getBoundBox(), vec3(6.0f, 3.0f, 3.0f)) || // back hangar
		  (jetPlane && (&jetPlane != cam->getPlane()) && checkTrivialCollision(myPos, myBoundBox, jetPlane->getPos(), jetPlane->getBoundBox())) || // jet plane
		  (fighterPlane && (&fighterPlane != cam->getPlane()) && checkTrivialCollision(myPos, myBoundBox, fighterPlane->getPos(), fighterPlane->getBoundBox())) || // fighter plane
		  (retroPlane && (&retroPlane != cam->getPlane()) && checkTrivialCollision(myPos, myBoundBox, retroPlane->getPos(), retroPlane->getBoundBox())) || // retro plane
		  (helicopter && checkTrivialCollision(myPos, myBoundBox, helicopter->getPos(), helicopter->getBoundBox()))) // helicopter
	{
		// too fast plane crashes
		if (cam->getPlane() && ((*cam->getPlane())->getCurrSpeed() >= 0.2f))
		{
			explosions.push_back(new Explosion((*cam->getPlane())->getPos(), vec3(0.0f), vec3(2.0f), vec3(0.0f), vec3(0.0f), 0.0f, 0.001f * (float)glutGet(GLUT_ELAPSED_TIME)));
			gameOver = new GameOver(cam->getPos() + 0.975f * cam->getDir(), vec3(0.0f), vec3(1.0f), vec3(0.0f), vec3(0.0f), 0.0f, 0.001f * (float)glutGet(GLUT_ELAPSED_TIME));
			state->setGameOver(true);
			flashlightOn = false;
			planeModeOn = false;

			deleteComponent(cam->getPlane());
			cam->setPlane(nullptr);

			return false;
		} // if

		return true;
	} // if

	return false;
} // CHECK COLLISIONS

// ========================================

/** Checks bounds of the world. */
bool checkBounds(Object **obj)
{
	// for real camera
	if (!planeModeOn && ((abs(cam->getPos().x) >= ISLAND_WIDTH) || (abs(cam->getPos().z) >= ISLAND_DEPTH)))
			return true;

	// ****************************************

	// for planes
	if (planeModeOn)
	{
		// bounds scene width
		if (abs((*obj)->getPos().x) > SCENE_WIDTH)
			(*obj)->setPos(vec3(-(*obj)->getPos().x, (*obj)->getPos().y, (*obj)->getPos().z));

		// bounds scene height
		(*obj)->setPos(vec3((*obj)->getPos().x, clamp((*obj)->getPos().y, (*obj)->getDefPos().y, 100.0f), (*obj)->getPos().z));

		// bounds scene depth
		if (abs((*obj)->getPos().z) > SCENE_DEPTH)
			(*obj)->setPos(vec3((*obj)->getPos().x, (*obj)->getPos().y, -(*obj)->getPos().z));

		// plane crashes outside the island with no altitude
		if (((abs((*obj)->getPos().x) >= (ISLAND_WIDTH + 3)) || (abs((*obj)->getPos().z) >= (ISLAND_DEPTH + 3))) &&
			  ((*obj)->getPos().y == (*obj)->getDefPos().y))
		{
			explosions.push_back(new Explosion((*obj)->getPos(), vec3(0.0f), vec3(2.0f), vec3(0.0f), vec3(0.0f), 0.0f, 0.001f * (float)glutGet(GLUT_ELAPSED_TIME)));
			gameOver = new GameOver(cam->getPos() + 0.975f * cam->getDir(), vec3(0.0f), vec3(1.0f), vec3(0.0f), vec3(0.0f), 0.0f, 0.001f * (float)glutGet(GLUT_ELAPSED_TIME));
			state->setGameOver(true);
			flashlightOn = false;
			planeModeOn = false;

			deleteComponent(obj);
			cam->setPlane(nullptr);

			return true;
		} // if
	} // if

	return false;
} // CHECK BOUNDS
