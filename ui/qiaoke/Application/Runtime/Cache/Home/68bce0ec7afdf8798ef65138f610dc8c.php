<?php if (!defined('THINK_PATH')) exit();?><!DOCTYPE html>
<!-- saved from url=(0023)http://job.bootcss.com/ -->
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
  
  <!--<script src="/__debug/Public/js/recom.js"></script> -->
  <script type="text/javascript">
  $(document).ready(function() {
     

  });
 
  </script>


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
  


  <div class="container"> 
    
   <div class="row"> 
    <div class="col-md-8" draggable="true"> 
     <div class="row"> 
      <div class="col-md-4" id="co_log" draggable="true"> 
       <div class="thumbnail"> 
        <img src="<?php echo ($job_info["logo"]); ?>" class="img-responsive" /> 
        <div class="caption"> 
         <h3><?php echo ($job_info["company"]); ?></h3> 
         <p draggable="true"><?php echo ($job_info["company_desc"]); ?></p> 
        </div> 
       </div> 
      </div> 
      <div class="col-md-8" id="job_title"> 
       <div class="row color_block"> 
        <div class="col-md-12 color_block"> 
         <div class="page-header"> 
          <h1><?php echo ($job_info["title"]); ?> <small><?php echo ($job_info["type"]); ?></small> </h1> 
         </div> 
        </div> 
        <div class="col-md-12 color_block"> 
         <div class="col-md-6"> 
          <p>工作地址：<?php echo ($job_info["location"]); ?></p> 
          <p>学历要求：<?php echo ($job_info["edu"]); ?></p> 
          <p>工作经验：<?php echo ($job_info["work_experience"]); ?></p> 薪资水平： 
          <span class="label label-danger"><?php echo ($job_info["salary"]); ?></span> 
         </div> 
         <div class="col-md-6"> 
          <div class="row"> 
           <?php if(is_array($job_info['label'])): foreach($job_info['label'] as $key=>$v): ?><span class="label label-success"><?php echo ($v); ?></span>&nbsp;&nbsp;<?php endforeach; endif; ?> 
           <p>发布时间：<?php echo ($job_info["pub_time"]); ?></p> 
           <p>招聘人数：<?php echo ($job_info["head_count"]); ?></p> 
           <p draggable="true">来源：<a href="<?php echo ($job_info["url"]); ?>"><?php echo ($job_info["src_desc"]); ?></a></p> 
           <a class="btn btn-block btn-primary" href="<?php echo ($job_info["url"]); ?>">申请职位</a> 
          </div> 
         </div> 
        </div> 
       </div> 
      </div> 
     </div> 
     <div class="row"> 
      <div class="col-md-12 color_block" id="job_detail"> 
       <blockquote> 
        <h3>职位需求：</h3> 
        <?php if(is_array($job_info['job_require'])): foreach($job_info['job_require'] as $key=>$job_require): ?><p> <?php echo ($job_require); ?></p><?php endforeach; endif; ?> 
        <h3>工作内容：</h3> 
        <?php if(is_array($job_info['job_duty'])): foreach($job_info['job_duty'] as $key=>$job_require): ?><p> <?php echo ($job_require); ?></p><?php endforeach; endif; ?> 
        <h3>公司福利：</h3> 
        <?php if(is_array($job_info['job_welfare'])): foreach($job_info['job_welfare'] as $key=>$job_require): ?><p> <?php echo ($job_require); ?></p><?php endforeach; endif; ?> 
        <h3>公司简介：</h3> 
        <?php if(is_array($job_info['company_desc'])): foreach($job_info['company_desc'] as $key=>$job_require): ?><p> <?php echo ($job_require); ?></p><?php endforeach; endif; ?> 
       </blockquote> 
      </div> 
     </div> 
    </div> 
    <!--
        推荐右侧
        --> 
    <div class="col-md-4 text-left"> 
     <div class="color_block row"> 
      <div class="col-md-12 text-center" draggable="true"> 
       <div class="panel panel-warning"> 
        <div class="panel-heading"> 
         <h3 class="panel-title">推荐阅读</h3> 
        </div> 
        <p> </p> 
        <div class="row">

        <?php if(is_array($recom_books)): foreach($recom_books as $key=>$books): ?><div class = "col-md-6">
         <a href="<?php echo ($books["url"]); ?>" title="<?php echo ($books["title"]); ?>"> <img src="<?php echo ($books["img"]); ?>" class="img-responsive img-thumbnail" width="217px" height="217px" /> </a> 
         <p><font color="red"><?php echo ($books["prize"]); ?></font> <?php echo ($books["orig_prize"]); ?> <br>
          <a class="btn" href="<?php echo ($books["url"]); ?>"><strong><?php echo ($books["website"]); ?></strong></a> 
         
            <img src="/__debug/Public/images/icon_star_2.gif"/>
            <img src="/__debug/Public/images/icon_star_2.gif"/>
            <img src="/__debug/Public/images/icon_star_2.gif"/>
            <img src="/__debug/Public/images/icon_star_2.gif"/>
            <img src="/__debug/Public/images/icon_star_1.gif"/></p>
            <p><?php echo ($books["desc"]); ?></p>
             <p></p>
           </div><?php endforeach; endif; ?> 
      </div>
       </div> 
      </div> 
     </div> 
    </div> 

     <div class="col-md-4 text-left">
      <!--这里可以添加更多推荐组件-->
      <script type="text/javascript" >
<!--
dd_ad_output="html";
dd_ad_width=180;
dd_ad_height=282;
dd_ad_client="P-322559";
dd_ad_format=20;
dd_ad_id=0;
dd_product_id=8904581;
dd_img_size=150;
dd_display_style=0;
dd_text_url="";
dd_color_text="";
dd_color_bg="";
dd_open_target="_blank";
dd_border=0;
dd_color_link="";
dd_ad_text="";
//--></script>
<script type="text/javascript" src="http://union.dangdang.com/union/script/dd_ads.js" ></script>

     </div>

   </div> 

   <div class="row"> 
    <div class="col-md-12" style="" draggable="true"> 
     <div class="panel panel-success"> 
      <div class="panel-heading"> 
       <h3 class="panel-title">推荐学习资料</h3> 
      </div> 
      <?php if(is_array($recom_links)): foreach($recom_links as $key=>$links): ?><a href="<?php echo ($links["url"]); ?>" class="list-group-item"> <h4 class="list-group-item-heading text-primary"> <?php echo ($links["title"]); ?> 
<img src="/__debug/Public/images/icon_star_2.gif"/>
          <img src="/__debug/Public/images/icon_star_2.gif"/>
           <img src="/__debug/Public/images/icon_star_2.gif"/>
            <img src="/__debug/Public/images/icon_star_2.gif"/>
             <img src="/__debug/Public/images/icon_star_1.gif"/>
       </h4> 
        <p class="list-group-item-text"> <?php echo ($links["desc"]); ?> </p> 

       </a> 

       <div> 
       </div><?php endforeach; endif; ?> 
     </div> 
     <div class="row"> 
      
      <?php if(is_array($recom_courses)): foreach($recom_courses as $key=>$courses): ?><div class="col-md-4 color_block"> 
        <div class="panel panel-success"> 
         <div class="panel-heading"> 
          <h3 class="panel-title">推荐课程 <small><?php echo ($courses["title"]); ?></small></h3> 
         </div> 
         <div class="embed-responsive embed-responsive-16by9"> 
          <iframe class="embed-responsive-item" src="<?php echo ($courses["play_url"]); ?>" allowfullscreen=""></iframe> 
         </div> 
         <div> 
          <h4>&nbsp;<?php echo ($courses["desc"]); ?></h4> 
          &nbsp;来源：<a href="<?php echo ($courses["url"]); ?>"><?php echo ($courses["website"]); ?></a> 
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;课程评价
          <img src="/__debug/Public/images/icon_star_2.gif"/>
          <img src="/__debug/Public/images/icon_star_2.gif"/>
           <img src="/__debug/Public/images/icon_star_2.gif"/>
            <img src="/__debug/Public/images/icon_star_1.gif"/>
             <img src="/__debug/Public/images/icon_star_1.gif"/>
         </div> 
        </div> 
       </div><?php endforeach; endif; ?> 
     </div> 
    </div> 
   </div> 
   <div class="col-md-12 color_b"> 
    <div> 
    </div> 
   </div> 
   <div class="col-md-12"> 
    <form role="form"> 
     <div class="form-group" draggable="true"> 
      <label class="control-label">你可以发布评论</label> 
      <textarea class="form-control input-lg"></textarea> 
      <a class="btn btn-info btn-sm"><span class="glyphicon glyphicon-star"></span>发布评论</a> 
     </div> 
    </form> 
   </div> 
   <div class="col-md-12"> 
    <ul class="media-list"> 
     <?php if(is_array($comments)): foreach($comments as $key=>$comment): ?><li class="media"> <a class="pull-left" href="<?php echo ($comment["user_profile_url"]); ?>"> <img class="media-object" src="<?php echo ($comment["avatar"]); ?>" href="<?php echo ($comment["user_profile_url"]); ?>" height="32px" width="32px" /></a> 
       <div class="media-body"> 
        <h5> <a href="<?php echo ($comment["user_profile_url"]); ?>"><?php echo ($comment["user_name"]); ?></a> <small> 发表于 <?php echo ($comment["pub_time"]); ?></small> </h5> 
        <p><?php echo ($comment["comment"]); ?></p> 
       </div> </li><?php endforeach; endif; ?> 
    </ul> 
   </div> 
  </div> 
  <div class="row"> 
   <div class="col-md-12"> 
   </div> 
  </div>  
  
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