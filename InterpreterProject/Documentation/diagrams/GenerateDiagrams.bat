REM generates images(.png) files for the list of plantuml diagrams.
java -jar ..\..\..\plantuml\plantuml.jar MainDiagram.txt
java -jar ..\..\..\plantuml\plantuml.jar NodesDiagram1.txt
java -jar ..\..\..\plantuml\plantuml.jar NodesDiagram2.txt
java -jar ..\..\..\plantuml\plantuml.jar NodesDiagram3.txt
move MainDiagram.png ..\images
move NodesDiagram1.png ..\images
move NodesDiagram2.png ..\images
move NodesDiagram3.png ..\images
start ..\images\MainDiagram.png
start ..\images\NodesDiagram1.png
start ..\images\NodesDiagram2.png
start ..\images\NodesDiagram3.png


