import firebase from 'firebase/app';
import 'firebase/firestore';

export const { Timestamp } = firebase.firestore;
// Initialize Firebase
if (!window.firebaseConfig) {
    window.firebaseConfig = window.electron.api.getFirebaseConfig()
}
export default firebase.initializeApp(window.firebaseConfig).firestore();