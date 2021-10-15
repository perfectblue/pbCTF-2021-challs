


import db from '../db/firestore';
import firebase from 'firebase/app';
import 'firebase/auth';

const createUserProfile = userProfile =>
  db
    .collection(`rooms/${userProfile.room}/profiles`)
    .doc(userProfile.uid)
    .set(userProfile)

export const getUserProfile = (room, uid) =>
  db
    .collection(`rooms/${room}/profiles`)
    .doc(uid)
    .get()
    .then(snanpshot => snanpshot.data())

export const getOrCreateAdminProfile = async (room, uid) => {
  const doc = db
    .collection(`rooms/${room}/profiles`)
    .doc(uid)
  if (await (doc.get()).exists) {
    await doc.delete();
  }
  const userProfile = {
    room,
    uid,
    admin: true,
    username: "Sauron",
    email: "Sauron@Mordor",
    avatar: "https://live.staticflickr.com/5475/11040203534_968078cb9e_t.jpg", // By Hersson Piratoba - https://www.flickr.com/photos/nossreh/11040203534 
    joinedChats: []
  }
  await createUserProfile(userProfile);
  return userProfile
}

export async function register({ room, email, password, username, avatar }) {
  const { user } = await firebase.auth().createUserWithEmailAndPassword(email, password);
  const userProfile = { room, uid: user.uid, username, email, avatar, joinedChats: [] }
  localStorage.setItem("room", userProfile.room);
  try {
    await createUserProfile(userProfile)
  } catch(e) {
    throw new Error("Error registering, room code might be invalid")
  }
  return userProfile;
}

export const login = async ({ room, email, password }) => {
  const { user } = await firebase.auth().signInWithEmailAndPassword(email, password);
  const userProfile = await getUserProfile(room, user.uid);
  if (!userProfile) {
    throw new Error("User is already signed up in another room")
  }
  localStorage.setItem("room", room);
  return userProfile;
}

export const admin = async ({ room, token }) => {
  const { user } = await firebase.auth().signInWithCustomToken(token);
  localStorage.setItem("room", room);
  const userProfile = await getOrCreateAdminProfile(room, user.uid);
  return userProfile;
}

export const logout = () => {
  localStorage.removeItem("room");
  firebase.auth().signOut()
}

export const onAuthStateChanges = onAuth =>
  firebase.auth().onAuthStateChanged(onAuth)
