<?php if (!defined('THINK_PATH')) exit();?><!DOCTYPE html>

<html lang="zh-cn">
 <head>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" /> 
  <!-- Meta, title, CSS, favicons, etc. --> 
  <meta charset="utf-8" /> 
  <meta http-equiv="X-UA-Compatible" content="IE=edge" /> 
  <meta name="viewport" content="width=device-width, initial-scale=1" /> 
  
  <!-- Bootstrap core CSS --> 
  <link href="/__debug/Public/css/app.min.css" rel="stylesheet" /> 
  <link href="/__debug/Public/css/font-awesome.min.css" rel="stylesheet" /> 
  <link href="/__debug/Public/css/bootstrap.min.css" rel="stylesheet" /> 
  <script src="/__debug/Public/js/jquery.min.js"></script> 
  <script src="/__debug/Public/js/bootstrap.min.js"></script> 
  <!-- HTML5 shim and Respond.js IE8 support of HTML5 elements and media queries --> 
  <!--[if lt IE 9]>
        <script src="http://cdn.bootcss.com/html5shiv/3.7.0/html5shiv.js"></script>
        <script src="http://cdn.bootcss.com/respond.js/1.4.2/respond.min.js"></script>
      <![endif]--> 
  <!-- Favicons --> 
  <link rel="apple-touch-icon-precomposed" href="./img/apple-touch-icon-precomposed.png" /> 
  <link rel="icon" href="/__debug/Public/images/favion.ico" /> 
  <script>
        var _hmt = _hmt || [];
      </script> 
  <script type="text/javascript">
        $(function () {
          $("#search").click(function(){
            
            var search_words = $('input[id=search_words]');
            
            var url = "<?php echo U('Home/Search/index/','','');?>";
            url += "&keywords=";
            url += search_words.val();
            //alert(url);
            window.location.href=url;

          });
        });
  </script>
  <script>
    var _hmt = _hmt || [];
    (function() {
      var hm = document.createElement("script");
      hm.src = "//hm.baidu.com/hm.js?225b31747f665ec848cbba3d4d9066e5";
      var s = document.getElementsByTagName("script")[0]; 
      s.parentNode.insertBefore(hm, s);
    })();
  </script>
 

  <meta name="description" content=<?php echo ($description); ?> /> 
  <meta name="keywords" content=<?php echo ($keywords); ?> /> 
  <title><?php echo ($title); ?></title> 

 </head> 
 <body> 

  <div id="navbar-top" class="navbar-top navbar navbar-default navbar-fixed-top"> 
   <div class="container"> 
    <div class="navbar-header"> 
     <a href="<?php echo U('Index/index');?>" class="navbar-brand">首页</a> 
     <a href="<?php echo U('About/index');?>" class="navbar-brand">关于</a> 
     <!--<img src="/assets/img/bl.png">--> 
    </div> 
    <p class="navbar-text navbar-right co-brand hidden-xs"><img src="/__debug/Public/images/logo2.png" /></p> 
   </div> 
  </div> 
  <header id="top-header" class="top-header jumbotron" style="background-image: url(/__debug/Public/images/header.jpg);"> 
   <div class="container"> 
    <div class="row"> 
     <div class="col-md-12"> 
      <div class="page-header"> 
       <h2>上翘课网，找工作不用愁<small> 职业从此与众不同</small></h2> 
       <p></p> 
       <form class="bs-example bs-example-form" role="form"> 
        <div class="row"> 
         <div class="col-lg-6 visible-lg"> 
          <div class="input-group "> 
           <input type="text" class="form-control" id="search_words" /> 
           <span class="input-group-btn"> <button class="btn btn-success" type="button" id="search"> 搜索职位 </button> </span>

          </div>
          <!-- /input-group --> 
         </div>
         <!-- /.col-lg-6 --> 
        </div>
        <!-- /.row --> 
       </form> 
      </div> 
      <div class="features media"> 
       <h5 class="media-left">快捷入口：</h5> 
       <div class="media-body"> 
        <p class="coms"> <a target="_blank" href="#" class="label label-default label-ali" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '阿里'])">阿里</a> 
         <!-- <a href="/company/腾讯" class="label label-default label-tencent" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '腾讯'])">腾讯</a> --> <a target="_blank" href="#" class="label label-default label-baidu" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '百度'])">百度</a> <a target="_blank" href="#" class="label label-default label-sina" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '金山'])">金山</a> <a target="_blank" href="#" class="label label-default label-sohu" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '迅雷'])">迅雷</a> <a target="_blank" href="#" class="label label-default label-360" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '360'])">360</a> 
         <!-- <a href="/company/小米" class="label label-default label-xiaomi" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '小米'])">小米</a> --> <a target="_blank" href="#" class="label label-default label-lenovo" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '畅游'])">畅游</a> </p> 
        <p class="salary"> <a href="#" class="label label-default" onclick="_hmt.push(['_trackEvent', 'salary', 'click', '10万起'])">10万起</a> <a href="#" class="label label-default" onclick="_hmt.push(['_trackEvent', 'salary', 'click', '20万起'])">20万起</a> <a href="#" class="label label-default" onclick="_hmt.push(['_trackEvent', 'salary', 'click', '30万起'])">30万起</a> <a href="#" class="label label-default" onclick="_hmt.push(['_trackEvent', 'salary', 'click', '50万起'])">50万起</a> <a href="#" class="label label-default" onclick="_hmt.push(['_trackEvent', 'salary', 'click', '土豪'])">土豪</a> </p> 
       </div> 
      </div> 
     </div> 
    </div> 
   </div> 
  </header> 
  
  <section class="newest-jobs container"> 
   <div class="page-header"> 
    <h3><?php echo ($head_title); ?>
     <small class="select-region pull-right">区域： <a href="#" onclick="_hmt.push(['_trackEvent', 'region', 'click', '全部'])">[全部]</a> <a href="#" onclick="_hmt.push(['_trackEvent', 'region', 'click', '北京'])">北京</a> <a href="#" onclick="_hmt.push(['_trackEvent', 'region', 'click', '广州'])">上海</a> <a href="#">广州</a> <a href="#" onclick="_hmt.push(['_trackEvent', 'region', 'click', '成都'])">深圳</a> <a href="#">成都</a> <a href="#" onclick="_hmt.push(['_trackEvent', 'region', 'click', '杭州'])">南京</a> <a href="#">杭州</a> 
      <div class="dropdown"> 
       <a class="btn dropdown-toggle" type="button" id="dropdownMenu1" data-toggle="dropdown"> 其他 <span class="caret"></span> </a> 
       <ul class="dropdown-menu" role="menu" aria-labelledby="dropdownMenu1"> 
        <li role="presentation"> <a role="menuitem" tabindex="-1" href="#" onclick="_hmt.push(['_trackEvent', 'region', 'click', '重庆'])">重庆</a> </li> 
        <li role="presentation"> <a role="menuitem" tabindex="-1" href="#" onclick="_hmt.push(['_trackEvent', 'region', 'click', '天津'])">天津</a> </li> 
        <li role="presentation"> <a role="menuitem" tabindex="-1" href="#" onclick="_hmt.push(['_trackEvent', 'region', 'click', '大连'])">大连</a> </li> 
       </ul> 
      </div> </small> </h3> 
   </div> 
</section> 

  <section class="job-list"> 
   <div class="container"> 
    <div class="row"> 
     <?php if(is_array($hot_job_list)): foreach($hot_job_list as $key=>$items): ?><div class="col-sm-6 col-md-4"> 
       <a href="<?php echo ($items["redirect"]); ?>" class="job-item-wrap" title="<?php echo ($items["title"]); ?>" target="_blank"> 
        <div class="job-item"> 
         <div class="job-source light-green"> 
          <img class="img-responsive" src="<?php echo ($items["logo"]); ?>" /> 
         </div> 
         <div class="job-company">
          <?php echo ($items["company"]); ?>
         </div> 
         <div class="job-title">
          <?php echo ($items["title"]); ?>
         </div> 
         <div class="job-salary">
          <?php echo ($items["salary"]); ?>
         </div> 
         <div class="job-comments"> 
          <p> 
           <?php if(is_array($items['label'])): foreach($items['label'] as $key=>$v): ?><span class="label label-default"><?php echo ($v); ?></span><?php endforeach; endif; ?> </p> 
         </div> 
         <div class="job-meta"> 
          <span class="job-location"><?php echo ($items["location"]); ?></span> 
          <span class="job-publish-time"><?php echo ($items["pub_time"]); ?></span> 
         </div> 
        </div> </a> 
      </div><?php endforeach; endif; ?> 
    </div>
    <!-- .row --> 
    <div class="col-sm-6 col-sm-push-3 col-md-4 col-md-push-4"> 
     <p> <a class="btn btn-primary btn-lg btn-block" href="#"><i class="fa fa-th"></i> 查看全部职位</a> </p> 
     
    </div> 
   </div>
   <!-- .container --> 
  </section> 
  
  <footer class="footer " style="background-image: url(/__debug/Public/images/footer.jpg);"> 
   <div class="container"> 
    <div class="row footer-top"> 
     <div class="col-sm-6 col-lg-6"> 
      <h4> <img src="/__debug/Public/images/logo.png" /> </h4> 
      <h4><a href="#">翘课网</a><small>提供最精致的职业教育推荐服务</small></h4> 
      <p>通向职业 之路&nbsp;&nbsp;<img src="/__debug/Public/images/marker_red_sprite.png"/></p>
     </div> 
     <div class="col-sm-6  col-lg-5 col-lg-offset-1"> 
      <div class="row about"> 
       <div class="col-xs-4 col-md-3"> 
        <h4>关于</h4> 
        <ul class="list-unstyled"> 
         <li><a href="#">关于我们</a></li> 
         <li><a href="#">广告合作</a></li> 
         <li><a href="#">友情链接</a></li> 
        </ul> 
       </div> 
       <div class="col-xs-4 col-md-3"> 
        <h4>联系方式</h4> 
        <ul class="list-unstyled"> 
         <li><a href="http://weibo.com/warlife" title="逃课网官方微博" target="_blank">新浪微博</a></li> 
         <li><a href="mailto:justastriver@qq.com">电子邮件</a></li> 
        </ul> 
       </div> 
       <div class="col-xs-4 col-md-3"> 
        <h4>旗下网站</h4> 
        <ul class="list-unstyled"> 
         <li><a href="http://www.ren58.com/" target="_blank">人物世界</a></li> 
         <li><a href="http://www.pipichong.com/" target="_blank">爱美社区</a></li> 
        </ul> 
       </div> 
       <div class="col-md-3 hidden-xs"> 
        <h4>赞助商</h4> 
        <ul class="list-unstyled"> 
         <li><a href="http://www.baidu.cn/" target="_blank">百度</a></li> 
         <li><a href="https://www.ren58.com/" target="_blank">佳职联诚官网</a></li> 
        </ul> 
       </div> 
      </div> 
     </div> 
    </div> 
    <hr /> 
    <div class="row footer-bottom"> 
     <ul class="list-inline text-center"> 
      <li><a href="http://www.miibeian.gov.cn/" target="_blank">京ICP备14006962号-2</a></li> 
     </ul> 
    </div> 
   </div> 
  </footer> 
  <script src="./js/jquery.min.js"></script> 
  <script src="./js/bootstrap.min.js"></script> 
  <script src="./js/jquery.scrollUp.min.js"></script> 
  <script>
      ;(function(window, document, $){
        // $(document).ready(function(){
        //   $.adaptiveBackground.run()
        // });

        $.scrollUp({
              scrollName: 'scrollUp', // Element ID
              topDistance: '300', // Distance from top before showing element (px)
              topSpeed: 300, // Speed back to top (ms)
              animation: 'fade', // Fade, slide, none
              animationInSpeed: 200, // Animation in speed (ms)
              animationOutSpeed: 200, // Animation out speed (ms)
              scrollText: '<i class="fa fa-angle-up"></i>', // Text for element
              activeOverlay: false  // Set CSS color to display scrollUp active point, e.g '#00FFFF'
        });

        $(window).scroll(function() {
            if ($("#navbar-top").offset().top > 300) {
                $('.co-brand > img').attr('src', '/__debug/Public/images/logo2.png');
            } else {
                $('.co-brand > img').attr('src', '/__debug/Public/images/logo2.png');
            }
        });
      })(window, document, jQuery)
      </script>
  <a id="scrollUp" href="#" style="position: fixed; z-index: 2147483647; display: none;"> <i class="fa fa-angle-up"></i> </a> 
  <!--
      <script type="text/javascript">
      var _bdhmProtocol = (("https:" == document.location.protocol) ? " https://" : " http://");
      document.write(unescape("%3Cscript src='" + _bdhmProtocol + "hm.baidu.com/h.js%3Fd979e0116b6882c9cdc4cf2c8467d312' type='text/javascript'%3E%3C/script%3E"));
      </script>
      <script src="./js/h.js" type="text/javascript"></script>
      --> 

 </body>
</html>