/*global THREE, requestAnimationFrame, console*/

var clock = new THREE.Clock();
var speed = 10;
var rotationSpeed = 1;

var camera, scene, renderer;

var geometry, material, mesh;

var camera1, camera2, camera3;


var key1 = true;
var key2 = false;
var key3 = false;
var key4 = false;

var arrowUpx = false;
var arrowDownx = false;
var arrowUpy = false;
var arrowDowny = false;
var arrowUpz = false;
var arrowDownz = false;
var V1right = false;
var V1left = false;
var V2right = false;
var V2left = false;
var V3right = false;
var V3left = false;

var elements = new Array();  //stores all elements

var elementAux;


var v0, v1, v2, v3; //parent elements

var wireframe = true;


function createElement1(x ,y ,z){
    'use strict';
    var element = new THREE.Object3D();

    //primeira piramide
    material = new THREE.MeshBasicMaterial({ color: 0xcb210a, wireframe: wireframe });
    geometry = new THREE.ConeGeometry( 5, 20, 4 );
    elementAux = new THREE.Mesh(geometry, material);
    elementAux.position.set(0, 0, 0);
    elements.push(elementAux);
    element.add(elementAux);


    //segunda piramide
    material = new THREE.MeshBasicMaterial({ color: 0xcb210a, wireframe: wireframe });
    geometry = new THREE.ConeGeometry( 5, 20, 4 );
    elementAux = new THREE.Mesh(geometry, material);
    elementAux.position.set(0, 10, 0);
    elements.push(elementAux);
    element.add(elementAux);
    
    element.position.set(x, y, z);
    element.rotateOnAxis ( new THREE.Vector3(1,0,0), 5); 
    
    return element
}

function createElement2(x, y, z){
    'use strict'
    var element = new THREE.Object3D();

    //torus
    elementAux = new THREE.Mesh(new THREE.TorusGeometry( 5, 1, 8, 50 ), new THREE.MeshBasicMaterial( {color: 0x4c5678, wireframe: wireframe} ));
    elementAux.position.set(0, 0, 0);
    elements.push(elementAux);
    element.add(elementAux);

    //octahedron
    elementAux = new THREE.Mesh(new THREE.OctahedronGeometry(2, 0), new THREE.MeshBasicMaterial( {color: 0xdb5717, wireframe: wireframe} ));
    elementAux.position.set(0, 0, 0);
    elements.push(elementAux);
    element.add(elementAux);

    element.position.set(x, y, z);

    return element;
}


function createScene() {
    'use strict';
    
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0xe6d0b8);
    
    //v0
    v0 = createElement1(-5, 20, 6);
    scene.add(v0);


    //v1
    v1 = new THREE.Object3D();
    elementAux = new THREE.Mesh(new THREE.SphereGeometry(2, 10, 10), new THREE.MeshBasicMaterial({ color: 0xd5af63, wireframe: wireframe }));
    elements.push(elementAux);
    v1.add(elementAux);
    v1.position.set(15,2,-8);    
    v0.add(v1);

    elementAux = new THREE.Mesh(new THREE.BoxGeometry( 3, 3, 6 ), new THREE.MeshBasicMaterial( {color: 0x4d2818, wireframe: wireframe} ));
    elementAux.position.set(50, 0, 0);
    elements.push(elementAux);
    v1.add(elementAux);

    elementAux = new THREE.Mesh(new THREE.CylinderGeometry( 4, 4, 16, 16 ), new THREE.MeshBasicMaterial( {color: 0xf3f1f1, wireframe: wireframe} ));
    elementAux.position.set(16, 19, 0);
    elements.push(elementAux);
    v1.add(elementAux);
    
    

    //v2
    v2 = new THREE.Object3D();    
    elementAux = new THREE.Mesh(new THREE.ConeGeometry(3, 8, 20), new THREE.MeshBasicMaterial({ color: 0x020001, wireframe: wireframe }));
    elements.push(elementAux);
    v2.add(elementAux);
    v2.position.set(20, 0, 17);    
    v1.add(v2);


    elementAux = new THREE.Mesh(new THREE.BoxGeometry( 3, 6, 3 ), new THREE.MeshBasicMaterial( {color: 0x549781, wireframe: wireframe} ));
    elementAux.position.set(-24, -6, 0);
    elements.push(elementAux);
    v2.add(elementAux);

    elementAux = new THREE.Mesh(new THREE.BoxGeometry( 6, 3, 3 ), new THREE.MeshBasicMaterial( {color: 0xf3f1f1, wireframe: wireframe} ));
    elementAux.position.set(-16, -12, 0);
    elements.push(elementAux);
    v2.add(elementAux);

    elementAux = new THREE.Mesh(new THREE.BoxGeometry( 3, 3, 6 ), new THREE.MeshBasicMaterial( {color: 0x4c5678, wireframe: wireframe} ));
    elementAux.position.set(-20, -18, -2);
    elements.push(elementAux);
    v2.add(elementAux);

    elementAux = createElement2(-10, -1, 15);
    v2.add(elementAux);

    elementAux = new THREE.Mesh(new THREE.CylinderGeometry(4, 4, 6, 3 ), new THREE.MeshBasicMaterial({ color: 0xdb5717, wireframe: wireframe }));
    elementAux.position.set(-50, -4, 4);
    elements.push(elementAux);
    v2.add(elementAux);
    

    
    //v3
    v3 = new THREE.Object3D();
    elementAux = new THREE.Mesh(new THREE.SphereGeometry(2, 10, 10), new THREE.MeshBasicMaterial({ color: 0xd5af63, wireframe: wireframe }));
    elements.push(elementAux);
    v3.add(elementAux);
    v3.position.set(10, 2, -4);
    v2.add(v3);

    
    elementAux = new THREE.Mesh(new THREE.TorusGeometry( 5, 1, 8, 50, 3 ), new THREE.MeshBasicMaterial( {color: 0x4d2818, wireframe: wireframe} ));
    elementAux.position.set(-20, -4, -8);
    elements.push(elementAux);
    v3.add(elementAux);

    elementAux = new THREE.Mesh(new THREE.SphereGeometry(2, 10, 10), new THREE.MeshBasicMaterial({ color: 0xf3f1f1, wireframe: wireframe }));
    elementAux.position.set(4, 4, -6);
    elements.push(elementAux);
    v3.add(elementAux);

    elementAux = new THREE.Mesh(new THREE.CylinderGeometry(4, 2, 3, 20), new THREE.MeshBasicMaterial({ color: 0xd5af63, wireframe: wireframe }));
    elementAux.position.set(20, 10, -10);
    elements.push(elementAux);
    v3.add(elementAux);

    elementAux = new THREE.Mesh(new THREE.BoxGeometry( 2, 2, 10 ), new THREE.MeshBasicMaterial( {color: 0x020001, wireframe: wireframe} ));
    elementAux.position.set(-10, -2, -20);
    elements.push(elementAux);
    v3.add(elementAux);

    elementAux = new THREE.Mesh(new THREE.BoxGeometry( 4, 4, 4 ), new THREE.MeshBasicMaterial( {color: 0x549781, wireframe: wireframe} ));
    elementAux.position.set(0, -2, -15);
    elements.push(elementAux);
    v3.add(elementAux);

    
    var geom = new THREE.Geometry();
    var triangle = new THREE.Triangle(new THREE.Vector3(0, 0, 0), new THREE.Vector3(10, 0, 0), new THREE.Vector3(10, 15, 0));
    geom.vertices.push(triangle.a);
    geom.vertices.push(triangle.b);
    geom.vertices.push(triangle.c);
    geom.faces.push(new THREE.Face3(0, 1, 2, triangle.normal()));
    elementAux = new THREE.Mesh(geom, new THREE.MeshBasicMaterial( {color: 0xcb210a, wireframe: wireframe} ));
    elements.push(elementAux);
    v3.add(elementAux);   

}

function createPrespectiveCamera() {
    'use strict';
    camera = new THREE.PerspectiveCamera(70,
                                         window.innerWidth / window.innerHeight,
                                         1,
                                         1000);
    camera.position.x = 50;
    camera.position.y = 50;
    camera.position.z = 50;
    camera.lookAt(scene.position);
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

    renderer.setSize(window.innerWidth, window.innerHeight);
    
    if (window.innerHeight > 0 && window.innerWidth > 0) {
        camera.aspect = window.innerWidth / window.innerHeight;
        camera.updateProjectionMatrix();
    }


}

function onkeyUp(e) {
    'use strict';

    switch (e.keyCode){

    case 52:  //4
        key4 = true;
        break;
    case 38:  //arrowUpx
        arrowUpx = false;
        break;
    case 40:  //arrowDownx
        arrowDownx = false;
        break;
    case 37:  //arrowLeft
        arrowUpy = false;
        break;
    case 39:  //arrowRight
        arrowDowny = false;
        break;
    case 68:  //d
        arrowUpz = false;
        break;
    case 67:  //c
        arrowDownz = false;
        break;
    case 81:  //q
        V1left = false;
        break;
    case 87:  //w
        V1right = false;
        break;
    case 65:  //a
        V2left = false;
        break;
    case 83:  //s
        V2right = false;
        break;
    case 88:  //x
        V3left = false;
        break;
    case 90:  //z
        V3right = false;
        break;
    }
}

function onKeyDown(e) {
    'use strict';
    
    switch (e.keyCode) {

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
    case 51:  //2
        key1 = false;
        key2 = false;
        key3 = true;
        break;
    case 38:  //arrowUpx
        arrowUpx = true;
        break;
    case 40:  //arrowDownx
        arrowDownx = true;
        break;
    case 37:  //arrowLeft
        arrowUpy = true;
        break;
    case 39:  //arrowRight
        arrowDowny = true;
        break;
    case 68:  //d
        arrowUpz = true;
        break;
    case 67:  //c
        arrowDownz = true;
        break;
    case 81:  //q
        V1left = true;
        break;
    case 87:  //w
        V1right = true;
        break;
    case 65:  //a
        V2left = true;
        break;
    case 83:  //s
        V2right = true;
        break;
    case 88:  //x
        V3left = true;
        break;
    case 90:  //z
        V3right = true;
        break;
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
    createPrespectiveCamera();
    camera1 = createOrthographicCamera(50, 0, 0);
    camera2 = createOrthographicCamera(0, 50, 0);
    camera3 = createOrthographicCamera(0, 0, 50);
    
    
    window.addEventListener("keydown", onKeyDown);
    window.addEventListener("resize", onResize);
    window.addEventListener("keyup", onkeyUp);

    clock.start();

}

function moveObj(obj, delta) {
    var distance = delta*speed;

    var x, y, z;
    if(arrowDownx && !arrowUpx){
        x = -1;
    }else if(!arrowDownx && arrowUpx){
        x = 1;
    }else{
        x = 0;
    } 

    if(arrowDowny && !arrowUpy){
        y = -1;
    }else if(!arrowDowny && arrowUpy){
        y = 1;
    }else{
        y = 0;
    } 

    if(arrowDownz && !arrowUpz){
        z = -1;
    }else if(!arrowDownz && arrowUpz){
        z = 1;
    }else{
        z = 0;
    } 

    obj.position.x += x*distance;
    obj.position.y += y*distance;
    obj.position.z += z*distance;

}

function rotateObj(obj, axis, delta, left, right) {
    var signal = 0;
    if(left && !right){
        signal = 1;
    }else if(!left && right){
        signal = -1;
    }
    
    var q = new THREE.Quaternion();
    q.setFromAxisAngle(axis, signal*delta*rotationSpeed);

    obj.position.applyQuaternion(q);
}

function selectCamera(){
    if(key1){
        return camera1;
    }else if(key2){
        return camera2;
    }else{
        return camera3
    }
}


function updateMaterial(){
    if(key4){
        for(let i=0; i<elements.length ; i++){
            elements[i].material.wireframe = !elements[i].material.wireframe;
        }
        key4 = false;
    }
}


function animate() {
    'use strict';

    //update
    var delta = clock.getDelta();
    rotateObj(v1, new THREE.Vector3(1,0,0), delta, V1left, V1right);   
    rotateObj(v2, new THREE.Vector3(1,0,0), delta, V2left, V2right); 
    rotateObj(v3, new THREE.Vector3(0,1,0), delta, V3left, V3right);

    moveObj(v0, delta);
    

    updateMaterial();
      
    //display
    render(selectCamera());

    
    requestAnimationFrame(animate);
}

