/*global THREE, requestAnimationFrame, console*/

//const { DirectionalLight } = require("three");

var clock = new THREE.Clock();
const speed = 1;


var scene, renderer;
var pauseScene;

var camera1, camera2, pauseCamera;

var geometry, material, mesh;

var key1 = true;
var key2 = false;
var spacebar = false;


var Q = false;
var W = false;
var E = false;
var R = false;
var T = false;
var Y = false;
var A = false;

var d_light = true;
var s_light_1 = true;
var s_light_2 = true;
var s_light_3 = true;

var dir_light, dir_light_helper;
var spot_light_1, spot_light_2, spot_light_3;
var spot_light_1_helper, spot_light_2_helper, spot_light_3_helper;

var floor, degree1, degree2;

var texture;
var origami1, origami2, origami3;

var elements = new Array(); 
var material = 0;

function createFloor() {
    var floor = new THREE.Object3D();

    geometry = new THREE.BoxGeometry(window.innerWidth, 1, window.innerHeight);
    material = new THREE.MeshPhongMaterial( {color: 0x898786, side: THREE.DoubleSide} );
    mesh = new THREE.Mesh( geometry, material );
    
    floor.add(mesh);

    floor.position.set(0, -25, 0);
    elements.push(floor);

    return floor;
}

function createDegree1() {
    var degree = new THREE.Object3D();

    geometry = new THREE.BoxGeometry(120, 6, 120);
    material = new THREE.MeshPhongMaterial( {color: 0x7a615f} );
    cube = new THREE.Mesh( geometry, material );
    degree.add( cube );

    degree.position.set(0, -21.5, 0);
    elements.push(degree);

    return degree;
}

function createDegree2() {
    var degree = new THREE.Object3D();

    geometry = new THREE.BoxGeometry(110, 6, 110);
    material = new THREE.MeshPhongMaterial( {color: 0x7a615f} );
    cube = new THREE.Mesh( geometry, material );
    degree.add( cube );

    degree.position.set(0, -15.5, 0);
    elements.push(degree);

    return degree;
}

function origami1(x,y,z){

    object = new THREE.Object3D();
    var geometry, vertices, material, mesh;

    //metade da folha
    
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         -10.0,  10.0,  0.0,
         10.0,  10.0,  1.0,
         10.0, -10.0,  0.0
    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );

    object.add(mesh);

    //metade da folha
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
         -10.0, -10.0,  1.0,
         -10.0, 10.0,  0.0,
         10.0,  -10.0,  0.0
 
    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );

    object.add(mesh);

    object.position.set(x, y, z);

    return object;

}

function origami2(x,y,z){
    object = new THREE.Object3D();
    var geometry, vertices, material, mesh;

    //papagaio
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         -5.0,  0.0,  0.0,
         5.0,  0.0,  0.0,
         0.0, 5.0,  0.0,

         -5.0,  0.0,  0.0,
         5.0,  0.0,  0.0,
         0.0, -25.0,  0.0

    ] );


    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

        

    //back
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  -25.0,  0.0,
         5.0,  -4.0,  0.0,
         0.5, -4.0,  1.0,


    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  -25.0,  0.0,
         -5.0,  -4.0,  0.0,
         -0.5, -4.0,  1.0,


    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    //front under
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  -25.0,  0.0,
         0.2,  -2.0,  0.2,
         5.0, 0.0,  0.0,


    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  -25.0,  0.0,
         -0.2,  -2.0,  0.2,
         -5.0, 0.0,  0.0,


    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    //front top
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  -25.0,  0.0,
         0.2,  -2.0,  0.2,
         5.0,  -4.0,  0.0,


    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  -25.0,  0.0,
         -0.2,  -2.0,  0.2,
         -5.0,  -4.0,  0.0,


    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);


    object.position.set(x, y, z);

    return object;

}

function origami3(x, y, z){
    object = new THREE.Object3D();
    var geometry, vertices, material, mesh;

    //body
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  0.0,  4.0,
         -2.0, 3.0,  0.0,
         9.0, 0.0,  4.0,
         
         -2.0, 3.0,  0.0,
         9.0, 0.0,  4.0,
         13.0, 6.0, 0.0

    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  0.0,  -4.0,
         -2.0, 3.0,  0.0,
         9.0, 0.0,  -4.0,
         
         -2.0, 3.0,  0.0,
         9.0, 0.0, -4.0,
         13.0, 6.0, 0.0

    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    //neck
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  0.0,  4.0,
         -2.0, 3.0,  0.0,
         5.0, 11.0,  1.0,
         
         -2.0, 3.0,  0.0,
         5.0, 11.0,  1.0,
         4.5, 12.0, 0.0

    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  0.0,  -4.0,
         -2.0, 3.0,  0.0,
         5.0, 11.0,  -1.0,
         
         -2.0, 3.0,  0.0,
         5.0, 11.0,  -1.0,
         4.5, 12.0, 0.0

    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    //wings top
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  0.0,  -4.0,
         -2.0, 3.0,  0.0,
         6.0, 4.0,  -1.0,
         
         0.0, 0.0,  -4.0,
         6.0, 4.0, -1.0,
         5.0, -0.5, -4.0

    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  0.0,  4.0,
         -2.0, 3.0,  0.0,
         6.0, 4.0,  1.0,
         
         0.0, 0.0,  4.0,
         6.0, 4.0, 1.0,
         5.0, -0.5, 4.0

    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);


    //wings inside
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  0.0,  -4.0,
         -2.0, 3.0,  0.0,
         6.0, 4.0,  -1.0,
         
         0.0, 0.0,  -4.0,
         6.0, 4.0, -1.0,
         9.0, 0.0, -4.0

    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         0.0,  0.0,  4.0,
         -2.0, 3.0,  0.0,
         6.0, 4.0,  1.0,
         
         0.0, 0.0,  4.0,
         6.0, 4.0, 1.0,
         9.0, 0.0, 4.0

    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    //head
    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
         5.0, 11.0,  1.0,
         4.5, 12.0,  0.0,
         -2.0, 11.0,  0.0,         

    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    geometry = new THREE.BufferGeometry();
    vertices = new Float32Array( [
    
        5.0, 11.0,  -1.0,
        4.5, 12.0,  0.0,
        -2.0, 11.0,  0.0,  
         
    ] );
    geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
    material = new THREE.MeshPhongMaterial( { wireframe : false , map: texture, side:THREE.DoubleSide } );
    mesh = new THREE.Mesh( geometry, material );
    object.add(mesh);

    object.position.set(x, y, z);

    return object;


}

function lamp() {
    var lamp = new THREE.Object3D();
    var cone = new THREE.Mesh(new THREE.ConeGeometry(2, 5, 32), new THREE.MeshPhongMaterial({ color: 0xffffff}));
    var sphere = new THREE.Mesh(new THREE.SphereGeometry(1 , 32, 16), new THREE.MeshPhongMaterial({ color: 0x00ff00}));
    sphere.position.set(0,-2.5,0);

    lamp.add(cone);
    lamp.add(sphere);
    

    return lamp;
}

function activateDirectionalLight() {
    if (d_light) {
        dir_light.intensity = 1;
        scene.add(dir_light_helper);
    } else {
        dir_light.intensity = 0;
        scene.remove(dir_light_helper);
    }
}

function activateSpotLight1() {
    if (s_light_1) {
        spot_light_1.intensity = 3;
    } else {
        spot_light_1.intensity = 0;
    }
}

function activateSpotLight2() {
    if (s_light_2) {
        spot_light_2.intensity = 3;
    } else {
        spot_light_2.intensity = 0;
    }
}

function activateSpotLight3() {
    if (s_light_3) {
        spot_light_3.intensity = 3;
    } else {
        spot_light_3.intensity = 0;
    }
}

function createPauseScene(){
    'use strict'
    pauseScene = new THREE.Scene();
    pauseScene.background = new THREE.Color(0x97c3e9);

    const loader = new THREE.FontLoader();

    var text;
    loader.load( 'https://threejs.org/examples/fonts/helvetiker_regular.typeface.json', function ( font ) {

        text = new THREE.TextGeometry( 'Paused', {
            font: font,
            size: 100,
            height: 5,
            curveSegments: 12,
            bevelEnabled: true,
            bevelThickness: 10,
            bevelSize: 8,
            bevelOffset: 0,
            bevelSegments: 5
        } );
        
    });
    var textobj = new THREE.Mesh(text, new THREE.MeshBasicMaterial( {color: 0x00ff00} ));
    textobj.position.set(0,0,0);
    pauseScene.add(textobj);


}



function createScene() {
    'use strict';
    
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x97c3e9);
    //scene.add(new THREE.AxesHelper(10));



    floor = createFloor();
    scene.add(floor);

    degree1 = createDegree1();
    scene.add(degree1);

    degree2 = createDegree2();
    scene.add(degree2);

    texture = new THREE.TextureLoader().load( "js/pattern.jpg" );

    var geom = new THREE.Geometry();
    var triangle = new THREE.Triangle(new THREE.Vector3(0, 0, 0), new THREE.Vector3(10, 0, 0), new THREE.Vector3(10, 15, 0));
    geom.vertices.push(triangle.a);
    geom.vertices.push(triangle.b);
    geom.vertices.push(triangle.c);
    geom.faces.push(new THREE.Face3(0, 1, 2, triangle.normal()));


    origami1 = origami1(-30,10,10);
    scene.add(origami1);

    origami2 = origami2(0,20,10);
    scene.add(origami2);

    origami3 = origami3(30, 5, 10);
    scene.add(origami3);

    //ambient light
    /*const light = new THREE.AmbientLight(0xFFFFFF, 0.5);
    scene.add(light);*/

    dir_light = new THREE.DirectionalLight(0xff0000, 1);
    dir_light.position.set(-35, 40, 0);
    dir_light_helper = new THREE.DirectionalLightHelper(dir_light, 20);
    scene.add(dir_light, dir_light_helper);

    spot_light_1 = new THREE.SpotLight(0xffffff, 3, 100, Math.PI / 8, 0);
    spot_light_1.position.set(-40, 45, 10);
    spot_light_1.target.position.set(-40, 0, 0);
    scene.add(spot_light_1, spot_light_1.target);

    spot_light_2 = new THREE.SpotLight(0xffffff, 3, 100, Math.PI / 8, 0);
    spot_light_2.position.set(0, 45, 10);
    spot_light_2.target.position.set(0, 0, 0);
    scene.add(spot_light_2, spot_light_2.target);

    spot_light_3 = new THREE.SpotLight(0xffffff, 3, 100, Math.PI / 8, 0);
    spot_light_3.position.set(40, 45, 10);
    spot_light_3.target.position.set(35, 0, 0);
    scene.add(spot_light_3, spot_light_3.target);

    var projector1 = lamp();
    projector1.position.set(-35,47,0);
    scene.add(projector1);

    var projector2 = lamp();
    projector2.position.set(0,47,0);
    scene.add(projector2);

    var projector3 = lamp();
    projector3.position.set(35,47,0);
    scene.add(projector3);
}

function updateMaterial(){
    if(A){
        if(material == 0) {
            console.log("skfs");
            elements[0].material = new THREE.MeshLambertMaterial();
            elements[1].material = new THREE.MeshLambertMaterial();
            elements[2].material = new THREE.MeshLambertMaterial();
            material = 1
        } else {
            elements[0].material = new THREE.MeshPhongMaterial();        
            elements[1].material = new THREE.MeshPhongMaterial();
            elements[1].material = new THREE.MeshPhongMaterial();
            material = 0
        }
    }
    A = false;
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
    }
}

function onkeyUp(e) {
    'use strict';

    switch (e.keyCode){
    case 49:  //1
        key1 = true;
        key2 = false;  
        break;
    case 50:  //2
        key1 = false;
        key2 = true;
        break;
    case 81:  //q
        Q = false;
        break;
    case 87:  //w
        W = false;
        break;
    case 69:  //e
        E = false;
        break;
    case 82:  //r
        R = false;
        break;
    case 84:  //t
        T = false;
        break;
    case 89:  //y
        Y = false;
        break;
    case 65:  //A
        A = false;
        break;
    case 32://spacebar
        spacebar = !spacebar;
    }
}

function onKeyDown(e) {
    'use strict';
    
    switch (e.keyCode) {
    case 81:  //q
        Q = true;
        break;
    case 87:  //w
        W = true;
        break;
    case 69:  //e
        E = true;
        break;
    case 82:  //r
        R = true;
        break;
    case 84:  //t
        T = true;
        break;
    case 89:  //y
        Y = true;
        break;
    case 68: //d
        d_light = !d_light;
        break;
    case 90: //z
        s_light_1 = !s_light_1;
        break;
    case 88: //x
        s_light_2 = !s_light_2;
        break;
    case 67: //c
        s_light_3 = !s_light_3;
        break;
    case 65:  //A
        A = true;
        break;
    }
    
}

function selectCamera(){
    if(key1){
        return camera1;
    }else if(key2){
        return camera2;
    }
} 

function render(camera) {
    'use strict';
    renderer.autoClear = false;
    renderer.clear();
    renderer.render(scene, camera);
    if (spacebar){
        console.log("render pause");
        renderer.clearDepth();
        renderer.render(pauseScene, pauseCamera);
    }
    
}


function init() {
    'use strict';

    
    renderer = new THREE.WebGLRenderer({
        antialias: true
    });
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);
   
    createScene();
    createPauseScene();

    

    camera1 = createPerspectiveCamera(70, 35, 100);
    camera2 = createOrthographicCamera(0, 0, 50);
    pauseCamera = createPerspectiveCamera(70, 35, 100);
    
    window.addEventListener("keydown", onKeyDown);
    window.addEventListener("resize", onResize);
    window.addEventListener("keyup", onkeyUp);

    clock.start();

}

function rotateOrigamis(){
    var rot = clock.getDelta()*speed;
    if(Q){
        origami1.rotateY(rot);
    }
    if(W){
        origami1.rotateY(-rot);
    }
    if(E){
        origami2.rotateY(rot);
    }
    if(R){
        origami2.rotateY(-rot);
    }
    if(T){
        origami3.rotateY(rot);
    }
    if(Y){
        origami3.rotateY(-rot);
    }

}

function animate() {
    'use strict';

    //update
    if(!spacebar){
        rotateOrigamis();
    }
    activateDirectionalLight();
    activateSpotLight1();
    activateSpotLight2();
    activateSpotLight3();
    updateMaterial();

    //display
    render(selectCamera());
    
    requestAnimationFrame(animate);
}

