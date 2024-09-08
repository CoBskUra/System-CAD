
# Add source to this project's executable.
add_executable (MG1_projekt "Main.cpp" "Main.h" "Camera.cpp" "Camera.h"  "Constants.h" "resource.h" 
 "Figures/VirtualPoint.cpp" "Figures/VirtualPoint.h" "Figures/FigureType.h" "Figures/Torus.h" "Figures/Point.h" "Figures/InfinityGrid.cpp" "Figures/InfinityGrid.h" "Figures/Figure.cpp" "Figures/Figure.h" "Figures/Figure_ControlSelected.h" "Figures/Figure3D.cpp" "Figures/Figure3D.h" "Figures/CanterPoint.h" "Figures/Cursor.h" 
	"Figures/Bezier/BezierBase.cpp" "Figures/Bezier/BezierBase.h" "Figures/Bezier/BezierC0.cpp" "Figures/Bezier/BezierC0.h" "Figures/Bezier/BezierC2.cpp" "Figures/Bezier/BezierC2.h" "Figures/Bezier/BezierInterpolated.cpp" "Figures/Bezier/BezierInterpolated.h" "Figures/Bezier/Surface/BezierSurfaceC0.cpp" "Figures/Bezier/Surface/BezierSurfaceC0.h" "Figures/Bezier/Surface/BezierSurfaceC2.cpp" "Figures/Bezier/Surface/BezierSurfaceC2.h" "Figures/Bezier/Surface/BezierSurface.cpp" "Figures/Bezier/Surface/BezierSurface.h" 
	"Figures/FigureFunctions/MoveMeWithMouse.cpp" "Figures/FigureFunctions/MoveMeWithMouse.h" "Figures/FigureFunctions/StereoscoicView.cpp" "Figures/FigureFunctions/StereoscoicView.h" 
 "ShaderManadement/EBO.h" "ShaderManadement/Shader.h" "ShaderManadement/VAO.h" "ShaderManadement/VBO.h" 
 "ShaderManadement/shaderClass.cpp"  "ShaderRefrence/ShaderCreator.h" "ShaderRefrence/StaticShaders.cpp" "ShaderRefrence/StaticShaders.h" 
 "Container/FigureContainer.cpp" "Container/FigureContainer.h"
 "SceneManadement/Manager.cpp" "SceneManadement/Manager.h" "SceneManadement/Scene.cpp" "SceneManadement/Scene.h" "SceneManadement/SceneSerializer.h" "SceneManadement/SceneSerializer.cpp"
 "Helper/MathOperations.h"  "Helper/OpenGLHelper.h" "Helper/ImGuiExtenstions/imfilebrowser.h"
  "Transformations/Quaternion.cpp" "Transformations/Transpose.cpp" "Transformations/Transpose.h" "Transformations/TransposeImGui.h" "Transformations/Quaternion.h" "Transformations/QuaternionRotationImGui.cpp" "Transformations/QuaternionRotationImGui.h"
  "Transformations/Rotation.cpp" "Transformations/Rotation.h" "Transformations/RotationImGui.h" "Transformations/Scale.cpp" "Transformations/Scale.h" "Transformations/ScaleImGui.h"
    "IdGiver/I_IdGiver.h" "IdGiver/IdGiver.h" "IdGiver/IdGiver.cpp"  "Figures/FigureFunctions/MergePoints.h" "Figures/FigureFunctions/MergePoints.cpp" "Figures/GregoryPatch.h" "Figures/GregoryPatch.cpp")

  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
            -Wall
            -Wextra
            -Wshadow
            -Wconversion
            -Wfloat-equal
            -Wunreachable-code
            -Wnull-dereference
            -Wno-unused-parameter
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /permissive)
endif ()

  target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR} ".")


 file(COPY ${CMAKE_SOURCE_DIR}/resource DESTINATION ${CMAKE_BINARY_DIR})



target_link_libraries(MG1_projekt PRIVATE glm::glm)
target_link_libraries(MG1_projekt PRIVATE glad::glad)
target_link_libraries(MG1_projekt PRIVATE glfw)
target_link_libraries(MG1_projekt PRIVATE imgui::imgui)
# add Serializer library to project libraries; path to Serializer subproject built lib file
target_link_libraries(MG1_projekt PRIVATE MGSerializer)

# add header files to include search path
target_include_directories(MG1_projekt PRIVATE ../Scene-Serializer/cpp/Serializer/Serializer)


# Ustawienie katalogu z danymi
set(DATA_FILES_DIR "${CMAKE_SOURCE_DIR}/resource")

# Kopiowanie plików danych po zbudowaniu pliku wykonywalnego
add_custom_command(TARGET MG1_projekt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${DATA_FILES_DIR} "${CMAKE_BINARY_DIR}/resource")