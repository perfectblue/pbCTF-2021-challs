
import * as api from '../api/auth';

export const registerUser = formData => dispatch => {
  dispatch({type: 'AUTH_REGISTER_INIT'});
  return api.register(formData)
    .then(user => dispatch({type: 'AUTH_REGISTER_SUCCESS', user}))
    .catch(error => dispatch({type: 'AUTH_REGISTER_ERROR', error}))
}

export const loginUser = formData => dispatch => {
  dispatch({type: 'AUTH_LOGIN_INIT'});
  return api
    .login(formData)
    .then(user => dispatch({type: 'AUTH_LOGIN_SUCCESS', user}))
    .catch(error => {
      dispatch({type: 'AUTH_LOGIN_ERROR', error})
    })
}

export const loginAdmin = (room, token) => dispatch => {
  dispatch({type: 'AUTH_LOGIN_INIT'});
  return api
    .admin({room, token})
    .then(user => dispatch({type: 'AUTH_LOGIN_SUCCESS', user}))
    .catch(error => {
      dispatch({type: 'AUTH_LOGIN_ERROR', error})
    })
}

export const logout = () => dispatch =>
  api
    .logout()
    .then(_ => {
      dispatch({type: 'AUTH_LOGOUT_SUCCESS'});
      dispatch({type: 'CHATS_FETCH_RESTART'});
    })

export const listenToAuthChanges = () => dispatch => {
  dispatch({type: 'AUTH_ON_INIT'});
  return api.onAuthStateChanges(async authUser => {
    if (authUser && localStorage.getItem("room")) {
      const userProfile = await api.getUserProfile(localStorage.getItem("room"), authUser.uid);
      if (userProfile) {
        dispatch({type: 'AUTH_ON_SUCCESS', user: userProfile});
      } else {
        dispatch({type: 'AUTH_ON_ERROR'});
      }
    } else {
      dispatch({type: 'AUTH_ON_ERROR'});
    }
  })
}
