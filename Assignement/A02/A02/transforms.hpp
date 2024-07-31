

void SetTransform(A02 *A) {

	glm::mat4 M;

	M = glm::mat4(1.0f);
	A->SetMatrix(0,  M, false);

    M = glm::mat4(1.0f);			
	A->SetMatrix(1,  M, false);

	M = glm::translate(glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f)), glm::vec3(-2.0f, 0.0f, -1.0f));
	A->SetMatrix(2,  M, false);
	
	M = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
	A->SetMatrix(3,  M, false);
	
	M = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 1.0f)), glm::vec3(-2.0f, 0.0f, 1.0f));
	A->SetMatrix(4,  M, false);
	
	M = glm::translate(glm::mat4(1.0f),glm::vec3(4.0f, 0.0f, 4.0f));
	A->SetMatrix(5,  M, false);
	
	M = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.25f)), glm::vec3(0.0f, 0.0f, -1.0f));
	A->SetMatrix(6,  M, false);

	M = glm::translate(glm::scale(glm::shearX3D(glm::rotate(glm::mat4(1.0f), glm::radians(-26.5f), glm::vec3(0.0f, 1.0f, 0.0f)), 0.0f, -0.5f), glm::vec3(0.895f, 1.0f, 1.125f)),glm::vec3(2.0025f, 0.0f, -0.99f));
	A->SetMatrix(7,  M, false);
	
	M = glm::translate(glm::scale(glm::shearX3D(glm::rotate(glm::mat4(1.0f), glm::radians(63.25f), glm::vec3(0.0f, 1.0f, 0.0f)), 0.0f, -1.35f), glm::vec3(1.325f, 1.0f, 2.25f)), glm::vec3(-0.33f, 2.0f, 0.335f));
	A->SetMatrix(8,  M, false);
	
	M = glm::mat4(1.0f);
	A->SetMatrix(9,  M, false);
	
	M = glm::translate(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(-1.0f, 1.0f, 1.5f));
	A->SetMatrix(10, M, false);
	
	M = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(-2.705f, 2.0f, -0.88f));
	A->SetMatrix(11, M, false);
	
	M = glm::translate(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.5f)), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(1.0f, 2.0f, 0.0f));
	A->SetMatrix(12, M, false);
	
	M = glm::translate(glm::rotate(glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(-0.915f, -0.7075f, 0.915f));
	A->SetMatrix(13, M, false);
	
	M = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(150.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(2.233f, 2.0f, 0.13f));
	A->SetMatrix(14, M, false);
}
