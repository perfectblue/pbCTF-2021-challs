
import React from 'react';
import { useForm } from 'react-hook-form';
import { useDispatch, useSelector } from 'react-redux';
import { loginAdmin, loginUser } from '../actions/auth';
import LoadingView from './shared/LoadingView';
import { createChat } from '../actions/chats';

export default function LoginForm() {
  const { register, handleSubmit } = useForm();
  const dispatch = useDispatch();
  const error = useSelector(({ auth }) => auth.login.error)
  const isChecking = useSelector(({ auth }) => auth.login.isChecking)
  const user = useSelector(({ auth }) => auth.user);

  const onSubmit = data => {
    dispatch(loginUser(data));
  }

  if (isChecking) {
    return <LoadingView />
  }

  if (window.ADMIN_TOKEN && window.ADMIN_ROOM) {
    if (!error && !isChecking && !user) {
      dispatch(loginAdmin(window.ADMIN_ROOM, window.ADMIN_TOKEN)).then(() => dispatch(createChat({
        name: "Mordor",
        description: "A black, volcanic plain located in the southeast of Middle-earth to the East of Gondor, Ithilien and the great river Anduin",
        // By Christophe Auzeine - https://www.artstation.com/artwork/ayb2q
        image: "https://cdnb.artstation.com/p/assets/images/images/001/406/345/small/christophe-auzeine-mount-doom.jpg"
      }, window.ADMIN_ROOM))).then((chatId) => {
        window.open(`./index.html#/chat/${chatId}`, `chat-${chatId}`)
      })
    }
  }

  return (
    <form onSubmit={handleSubmit(onSubmit)} className="centered-container-form">
      <div className="header">Welcome Hobbit!</div>
      <div className="subheader">Login and visit other kingdoms!</div>
      <div className="form-container">
        <div className="form-group">
          <label htmlFor="room">Room Code</label>
          <input
            ref={register}
            type="text"
            className="form-control"
            name="room"
            id="room"
            minLength="32"
            maxLength="32"
            aria-describedby="codeHelp" />
          <small id="codeHelp" className="form-text text-muted">You need an room code to sign in</small>
        </div>

        <div className="form-group">
          <label htmlFor="email">Email</label>
          <input
            ref={register}
            type="email"
            className="form-control"
            id="email"
            name="email"
            aria-describedby="emailHelp" />
          <small id="emailHelp" className="form-text text-muted">We'll never share your email with any orcs.</small>
        </div>
        <div className="form-group">
          <label htmlFor="password">Password</label>
          <input
            ref={register}
            type="password"
            name="password"
            className="form-control"
            id="password" />
        </div>
        {error && <div className="alert alert-danger small">{error.message}</div>}
        <button
          type="submit"
          className="btn btn-outline-primary">Login</button>
      </div>
    </form>
  )
}
