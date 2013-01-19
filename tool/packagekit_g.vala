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
  public void resolve(string name, out bool installed, out string package_id)
    {
    var loop = new MainLoop();
    bool out_installed = false;
    string out_package_id = null;
    resolve_async.begin(name, (source, res) =>
      {
      try
        {
        resolve_async.end(res, out out_installed, out out_package_id);
        }
      catch(Error e)
        {
        stderr.printf("bad: %s\n", e.message);
        }
      loop.quit();
      });
    loop.run();
    installed = out_installed;
    package_id = out_package_id;
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
  private async void resolve_async(string name, out bool installed, out string package_id) throws IOError
    {
    Transaction transaction = Bus.get_proxy_sync(BusType.SYSTEM,
        "org.freedesktop.PackageKit", connection.get_tid());
    bool out_installed = false;
    string out_package_id = null;
    transaction.package.connect((info, package_id) =>
      {
      out_installed = info == "installed";
      out_package_id = package_id;
      });
    transaction.finished.connect((exit, runtime) =>
      {
      resolve_async.callback();
      });
    transaction.resolve("none", {name});
    yield;
    installed = out_installed;
    package_id = out_package_id;
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
