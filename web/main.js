// Initialize Firebase
let config = {
    apiKey: "AIzaSyBprZjdLFwBol1R-rT8eYTwOEB-dWDhN70",
    authDomain: "bachelorthesis-caec1.firebaseapp.com",
    databaseURL: "https://bachelorthesis-caec1.firebaseio.com",
    projectId: "bachelorthesis-caec1",
    storageBucket: "bachelorthesis-caec1.appspot.com",
    messagingSenderId: "610189267602"
};
firebase.initializeApp(config);

let database = firebase.database();

let lightsRef = database.ref('lights');

lightsRef.on('value', function(snapshot) {
    let value = snapshot.val();

    for (let key in value) {
        if (value.hasOwnProperty(key)) {
            console.log(key + " -> " + value[key]);
        }
    }
});
