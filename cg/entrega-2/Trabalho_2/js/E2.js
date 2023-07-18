/*global THREE, requestAnimationFrame, console*/

var clock = new THREE.Clock();
const speed = 1;


const planetRadius = 25; 
const rocketH = planetRadius/12;
const trashH = planetRadius/20;

var scene, renderer;

var camera1, camera2, camera3;

var geometry, material, mesh;

var key1 = true;
var key2 = false;
var key3 = false;

var arrowUp = false;
var arrowDown = false;
var arrowLeft = false;
var arrowRight = false;

var ship, trash;


var inverse = false;

var shipCosilisonSphere;

var movmentDirection = new THREE.Vector3(1, 0, 0);


function createShip(){
    var ship = new THREE.Object3D();

    geometry = new THREE.CylinderGeometry( 0.5, 0.5, rocketH/2, 32 );
    material = new THREE.MeshBasicMaterial( {color: 0xfddeca, wireframe: true} );
    mesh = new THREE.Mesh( geometry, material );
    mesh.position.set(0,0,0);
    ship.add(mesh);


    geometry = new THREE.CylinderGeometry( 0.5, 0, rocketH/2, 32 );
    material = new THREE.MeshBasicMaterial( {color: 0xe06666, wireframe: true} );
    mesh = new THREE.Mesh( geometry, material );
    mesh.position.set(0,-rocketH/2,0);
    ship.add(mesh);


    
    geometry = new THREE.CapsuleGeometry(0.3, rocketH/3, 10, 16);
    material = new THREE.MeshBasicMaterial( {color: 0xf6b26b} );
    mesh = new THREE.Mesh( geometry, material );
    mesh.position.set(rocketH/4,rocketH/4,-rocketH/4);
    ship.add( mesh );

    geometry = new THREE.CapsuleGeometry(0.3, rocketH/3, 10, 16);
    material = new THREE.MeshBasicMaterial( {color: 0xf6b26b} );
    mesh = new THREE.Mesh( geometry, material );
    mesh.position.set(-rocketH/4,rocketH/4,-rocketH/4);
    ship.add( mesh );

    geometry = new THREE.CapsuleGeometry(0.3, rocketH/3, 10, 16);
    material = new THREE.MeshBasicMaterial( {color: 0xf6b26b} );
    mesh = new THREE.Mesh( geometry, material );
    mesh.position.set(rocketH/4,rocketH/4,rocketH/4);
    ship.add( mesh );

    geometry = new THREE.CapsuleGeometry(0.3, rocketH/3, 10, 16);
    material = new THREE.MeshBasicMaterial( {color: 0xf6b26b} );
    mesh = new THREE.Mesh( geometry, material );
    mesh.position.set(-rocketH/4,rocketH/4,rocketH/4);
    ship.add( mesh );

    //colision sphere
    geometry = new THREE.SphereGeometry( rocketH/1.5, 32, 16 );
    material = new THREE.MeshBasicMaterial( {visible: false});
    mesh = new THREE.Mesh( geometry, material );

    ship.add(mesh);

    ship.position.setFromSpherical(new THREE.Spherical(planetRadius*1.2, Math.random()*Math.PI, Math.random()*Math.PI*2));
    ship.lookAt(0,0,0);


    return ship;
}

function createSpaceTrashCube() {
    var trash = new THREE.Object3D();

    geometry = new THREE.BoxGeometry(trashH, trashH, trashH);
    material = new THREE.MeshBasicMaterial( {color: 0xd4e8b0, wireframe: true} );
    mesh = new THREE.Mesh( geometry, material );
    mesh.position.set(0,0,0);
    trash.add(mesh);

    //colision sphere
    geometry = new THREE.SphereGeometry( trashH/2, 32, 16 );
    material = new THREE.MeshBasicMaterial( {visible: false});
    mesh = new THREE.Mesh( geometry, material );


    trash.add(mesh);

    trash.position.setFromSpherical(new THREE.Spherical(planetRadius*1.2, Math.random()*Math.PI, Math.random()*Math.PI*2));
    return trash;
}

function createSpaceTrashCone() {
    var trash = new THREE.Object3D();

    geometry = new THREE.ConeGeometry(trashH/2, trashH, 10);
    material = new THREE.MeshBasicMaterial( {color: 0xf6f685, wireframe: true} );
    mesh = new THREE.Mesh( geometry, material );
    mesh.position.set(0,0,0);
    trash.add(mesh);

    //colision sphere
    geometry = new THREE.SphereGeometry( trashH/2, 32, 16 );
    material = new THREE.MeshBasicMaterial( {visible: false});
    mesh = new THREE.Mesh( geometry, material );


    trash.add(mesh);

    trash.position.setFromSpherical(new THREE.Spherical(planetRadius*1.2, Math.random()*Math.PI, Math.random()*Math.PI*2));
    return trash;
}


function createSpaceTrashPolyhedron() {
    var trash = new THREE.Object3D();

    var verticesOfCube = [
        -1,-1,-1,    1,-1,-1,    1, 1,-1,    -1, 1,-1,
        -1,-1, 1,    1,-1, 1,    1, 1, 1,    -1, 1, 1,
    ];
    
    var indicesOfFaces = [
        2,1,0,    0,3,2,
        0,4,7,    7,3,0,
        0,1,5,    5,4,0,
        1,2,6,    6,5,1,
        2,3,7,    7,6,2,
        4,5,6,    6,7,4
    ];

    geometry = new THREE.PolyhedronGeometry(verticesOfCube, indicesOfFaces, trashH/2, 1);
    material = new THREE.MeshBasicMaterial( {color: 0xb7ffd0, wireframe: true} );
    mesh = new THREE.Mesh( geometry, material );
    mesh.position.set(0,0,0);
    trash.add(mesh);

    //colision sphere
    geometry = new THREE.SphereGeometry( trashH/2, 32, 16 );
    material = new THREE.MeshBasicMaterial( {visible: false});
    mesh = new THREE.Mesh( geometry, material );


    trash.add(mesh);

    trash.position.setFromSpherical(new THREE.Spherical(planetRadius*1.2, Math.random()*Math.PI, Math.random()*Math.PI*2));
    return trash;
}

function createScene() {
    'use strict';
    
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x17415f);
    scene.add(new THREE.AxesHelper(10));


    geometry = new THREE.SphereGeometry( planetRadius, 32, 16 );
    material = new THREE.MeshBasicMaterial( { color: 0x5b92e5, wireframe: true } );
    mesh = new THREE.Mesh( geometry, material );
    scene.add(mesh);

    ship = createShip();
    scene.add(ship);
  
    for(var i = 0; i < 6; i++) {
        trash = createSpaceTrashCube();
        scene.add(trash);
    }

    for(var i = 0; i < 7; i++) {
        trash = createSpaceTrashCone();
        scene.add(trash);
    }

    for(var i = 0; i < 7; i++) {
        trash = createSpaceTrashPolyhedron();
        scene.add(trash);
    }


}

function createPerspectiveCamera(x, y, z) {
    'use strict';
    var camera = new THREE.PerspectiveCamera(70,
                                         window.innerWidth / window.innerHeight,
                                         1,
                                         1000);
    camera.position.set(x,y,z);
    
    camera.lookAt(scene.position);
    
    

    return camera;
}

function createShipPerspectiveCamera(x, y, z) {
    'use strict';
    var camera = new THREE.PerspectiveCamera(70,
                                         window.innerWidth / window.innerHeight,
                                         1,
                                         1000);
    camera.position.set(0,-20,0);
    
    
    camera.lookAt(0,0,0);
    ship.add(camera);

    return camera;
}




function createOrthographicCamera(x, y, z) {
    'use strict';
    var newCamera = new THREE.OrthographicCamera(window.innerWidth / -20, window.innerWidth / 20, window.innerHeight / 20, window.innerHeight / -20, -10, 10000 );

    newCamera.position.x = x;
    newCamera.position.y = y;
    newCamera.position.z = z;
    newCamera.lookAt(scene.position);

    return newCamera;
}



function onResize() {
    'use strict';
    console.log("resize");

    renderer.setSize(window.innerWidth, window.innerHeight);
    
    if (window.innerHeight > 0 && window.innerWidth > 0) {
        selectCamera().aspect = window.innerWidth / window.innerHeight;
        selectCamera().updateProjectionMatrix();

        camera1 = new THREE.OrthographicCamera(window.innerWidth / -20, window.innerWidth / 20, window.innerHeight / 20, window.innerHeight / -20, -10, 10000 );
        camera2.aspect = window.innerWidth / window.innerHeight;
        camera2.updateProjectionMatrix();
        camera3.aspect = window.innerWidth / window.innerHeight;
        camera3.updateProjectionMatrix();

    }


}

function onkeyUp(e) {
    'use strict';

    switch (e.keyCode){
    case 49:  //1

        key1 = true;
        key2 = false;
        key3 = false;
        break;
    case 50:  //2

        key1 = false;
        key2 = true;
        key3 = false;
        break;
    case 51:  //3

        key1 = false;
        key2 = false;
        key3 = true;
        break;
    case 38:  //arrowUp
        arrowUp = false;
        break;
    case 40:  //arrowDown
        arrowDown = false;
        break;
    case 37:  //arrowLeft
        arrowLeft = false;
        break;
    case 39:  //arrowRight
        arrowRight = false;
        break;

    }
}

function onKeyDown(e) {
    'use strict';
    
    switch (e.keyCode) {
    case 38:  //arrowUp
        arrowUp = true;
        break;
    case 40:  //arrowDownx
        arrowDown = true;
        break;
    case 37:  //arrowLeft
        arrowLeft = true;
        break;
    case 39:  //arrowRight
        arrowRight = true;
        break;
    }
    
}

function selectCamera(){
    if(key1){
        return camera1;
    }else if(key2){
        return camera2;
    }else{
        return camera3;
    }
}

function render(camera) {
    'use strict';
    renderer.render(scene, camera);
    
}

function init() {
    'use strict';

    
    renderer = new THREE.WebGLRenderer({
        antialias: true
    });
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);
   
    createScene();

    camera1 = createOrthographicCamera(0, 0, 50);
    camera2 = createPerspectiveCamera(50, 50, 50);
    camera3 = createShipPerspectiveCamera(0, 0, 50);
    
    
    
    
    window.addEventListener("keydown", onKeyDown);
    window.addEventListener("resize", onResize);
    window.addEventListener("keyup", onkeyUp);

    clock.start();

}



function newCoordenates(delta, phi, theta) {
    var distance = delta*speed;


    var x, y;
    if(arrowDown && !arrowUp){
        x = 1;
    }else if(!arrowDown && arrowUp){
        x = -1;
    }else{
        x = 0;
    } 
  
    if(inverse){
        x = -x;
    }  

    if(arrowLeft && !arrowRight){
        y = -1;
    }else if(!arrowLeft && arrowRight){
        y = 1;
    }else{
        y = 0;
    } 

    var movement = new THREE.Vector2(x, y);
    movement.normalize();

    var newPhi = phi + distance*movement.x;
    var newTheta = theta + distance*movement.y;


    if( newPhi < 0){

        newPhi = 0.0001;
        inverse = !inverse;
        newTheta = Math.PI + newTheta;
    }
    if(newPhi > Math.PI){

        newPhi = Math.PI;
        inverse = !inverse;
        newTheta = Math.PI + newTheta;
    }

    var r = new THREE.Spherical(planetRadius*1.2, newPhi, newTheta);


    return r;
    
}

function sameQuadrant(a, b){
    var aPos = new THREE.Spherical().setFromVector3(a.position);
    var bPos = new THREE.Spherical().setFromVector3(b.position);


    
    if((aPos.phi < Math.PI/2 && bPos.phi > Math.PI/2) || (aPos.phi > Math.PI/2 && bPos.phi < Math.PI/2)){

        return false;
    }
    if((aPos.theta < 0 && bPos.theta > 0) || (aPos.theta > 0 && bPos.theta < 0)){

        return false;
    }



    return true;
}

function checkColisions(){
    
    
    for(let i=3; i<scene.children.length; i++){
        

        var current = scene.children[i];
        if(!sameQuadrant(ship, current)){
            continue;
        }

        var colisionSphere = current.children[current.children.length-1];
        var shipSphere = ship.children[ship.children.length-2];

        var dist = current.position.distanceTo(ship.position);

        
        if(dist < colisionSphere.geometry.parameters.radius + shipSphere.geometry.parameters.radius){

            scene.remove(current);
        }
        
        current.children[current.children.length-1].radius;

        
    }
}


function animate() {
    'use strict';

    //update
    var delta = clock.getDelta();
    var aux = new THREE.Spherical();
    var sphericalPos = aux.setFromVector3(ship.position);

    var newPos = newCoordenates(delta, sphericalPos.phi, sphericalPos.theta);   
    var aux = new THREE.Vector3().setFromSpherical(newPos);
    
    if(!aux.equals(ship.position)){
        camera3.lookAt(aux); 
    }   
    ship.position.setFromSpherical(newPos);
    ship.lookAt(0,0,0);

    checkColisions();


    //display
    render(selectCamera());
    
    requestAnimationFrame(animate);
}

