glm::mat4 MakeViewProjectionLookInDirection(glm::vec3 Pos, float Yaw, float Pitch, float Roll, float FOVy, float Ar, float nearPlane, float farPlane) {

	glm::mat4 Mprj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
	Mprj[1][1] *= -1;

	glm::mat4 Mv = glm::rotate(glm::mat4(1.0), -Roll, glm::vec3(0, 0, 1)) * glm::rotate(glm::mat4(1.0), -Pitch, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0), -Yaw, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), -Pos);
	
	glm::mat4 M = Mprj * Mv;
	return M;
}

glm::mat4 MakeViewProjectionLookAt(glm::vec3 Pos, glm::vec3 Target, glm::vec3 Up, float Roll, float FOVy, float Ar, float nearPlane, float farPlane) { 

	glm::mat4 Mprj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
	Mprj[1][1] *= -1;

	glm::mat4 Mv = glm::rotate(glm::mat4(1.0), -Roll, glm::vec3(0, 0, 1)) * glm::lookAt(Pos, Target, Up);
	
	glm::mat4 M = Mprj * Mv;
	return M;
}

glm::mat4 MakeWorld(glm::vec3 Pos, float Yaw, float Pitch, float Roll) {  

	glm::mat4 Mw = glm::translate(glm::mat4(1.0),Pos) * glm::rotate(glm::mat4(1.0), Yaw, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0), Pitch, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0), Roll, glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f));
	return Mw;
}
