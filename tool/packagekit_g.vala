/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

[DBus (name = "org.freedesktop.PackageKit")]
interface Connection : Object
  {
  public abstract string distro_id {owned get;}
  public abstract string get_tid() throws IOError;
  }

[DBus (name = "org.freedesktop.PackageKit.Transaction")]
interface Transaction : Object
  {
  public abstract void resolve(string filter, string[] packages) throws IOError;
  public abstract void install_packages(bool only_trusted, string[] package_ids) throws IOError;
  public signal void package(string info, string package_id, string summary);
  public signal void item_progress(string id, uint percentage);
  public signal void finished(string exit, uint runtime);
  }

public delegate void OnPackage(string info, string package_id);

public class Karrot.PackageKit
  {
  public PackageKit() throws IOError
    {
    connection = Bus.get_proxy_sync(BusType.SYSTEM,
      "org.freedesktop.PackageKit", "/org/freedesktop/PackageKit");
    }
  public string distro_id()
    {
    return connection.distro_id;
    }
  public void resolve(string name, OnPackage handler)
    {
    var loop = new MainLoop();
    resolve_async.begin(name, handler, (source, res) =>
      {
      try
        {
        resolve_async.end(res);
        }
      catch(Error e)
        {
        stderr.printf("bad: %s\n", e.message);
        }
      loop.quit();
      });
    loop.run();
    }
  public void install(string[] package_ids)
    {
    var loop = new MainLoop();
    install_async.begin(package_ids, (source, res) =>
      {
      try
        {
        install_async.end(res);
        }
      catch(Error e)
        {
        stderr.printf("bad: %s\n", e.message);
        }
      loop.quit();
      });
    loop.run();
    }
  private async void resolve_async(string name, OnPackage handler) throws IOError
    {
    Transaction transaction = Bus.get_proxy_sync(BusType.SYSTEM,
        "org.freedesktop.PackageKit", connection.get_tid());
    transaction.package.connect((info, package_id, summary) =>
      {
      handler(info, package_id);
      });
    transaction.finished.connect((exit, runtime) =>
      {
      resolve_async.callback();
      });
    transaction.resolve("none", {name});
    yield;
    }
  private async void install_async(string[] package_ids) throws IOError
    {
    Transaction transaction = Bus.get_proxy_sync(BusType.SYSTEM,
        "org.freedesktop.PackageKit", connection.get_tid());
    transaction.item_progress.connect((id, percent) =>
      {
      stdout.printf("%02d%% %s\n", (int)percent, id);
      });
    transaction.finished.connect((exit, runtime) =>
      {
      install_async.callback();
      });
    transaction.install_packages(true, package_ids);
    yield;
    }
  private Connection connection;
  }
