
import * as api from '../api/connection';

export const checkUserConnection = (room, uid) => dispatch =>
  api.onConnectionChanged((isConnected) => {
    api.setUserOnlineStatus(room, uid, isConnected)
    dispatch({type: 'CONNECTION_USER_STATUS_CHANGED'})
  })
